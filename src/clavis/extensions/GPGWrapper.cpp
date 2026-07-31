#include <extensions/GPGWrapper.h>

#include <iostream>
#include <gpgme.h>

#ifndef __WINDOWS__
#include <unistd.h>
#else
#include <io.h>
#endif

#include <error/ClavisError.h>
#include <system/Extensions.h>
#include <system/ProcessWrapper.h>

#include <language/Language.h>

#include <password_store/PasswordStore.h>

namespace Clavis {
    bool GPG::InitializeGPGME() {
#ifdef __MACOS__
        if (!System::FileExists(GetGpgAgentConfPath()))
            System::TryWriteFile(GetGpgAgentConfPath(), "pinentry-program /opt/homebrew/bin/pinentry-mac");
#endif

#ifdef __WINDOWS__
        // GPGME reads GPGME_DEBUG only during gpgme_check_version() (first call).
        // Set it before that call so the full internal trace goes to a log file.
        {
            static bool gpgmeDebugInit = false;
            if (!gpgmeDebugInit) {
                gpgmeDebugInit = true;
                auto logDir = System::GetAppDataFolder() / "gnupg";
                System::mkdir_p(logDir);
                auto logPath = logDir / "gpgme-debug.log";
                std::string val = "9:" + logPath.string();
                SetEnvironmentVariableA("GPGME_DEBUG", val.c_str());
                _putenv(("GPGME_DEBUG=" + val).c_str());
                std::cerr << "[GPGME debug log: " << logPath.string() << "]\n";
            }
        }

        // Locate gpg.exe: check alongside the executable first (installed / dev build
        // after CMake copies bundled executables), then fall back to searching PATH.
        std::filesystem::path gpgPath;
        {
            auto candidate = System::GetExecutableLocation() / "gpg.exe";
            if (System::FileExists(candidate)) {
                gpgPath = candidate;
            } else {
                char buf[MAX_PATH];
                if (SearchPathA(nullptr, "gpg.exe", nullptr, MAX_PATH, buf, nullptr) != 0)
                    gpgPath = buf;
            }
        }

        // GnuPG's home on Windows is %APPDATA%\gnupg, not ~/gnupg.
        auto homedirPath = System::GetAppDataFolder() / "gnupg";
        std::string homedirStr = homedirPath.string();

        // Set GNUPGHOME in both the Windows env block and the CRT's own env cache.
        // GPGME reads this via getenv() before falling back to gpgconf, so setting
        // it here prevents the gpgconf invocation for homedir discovery.
        SetEnvironmentVariableA("GNUPGHOME", homedirStr.c_str());
        _putenv(("GNUPGHOME=" + homedirStr).c_str());

        if (!gpgPath.empty()) {
            auto gpgDir = gpgPath.parent_path();
            std::string gpgDirStr = gpgDir.string();
            // Update PATH in both the Windows env block (inherited by child processes)
            // and the CRT cache (read by GPGME's getenv calls within this process).
            char* existingPath = getenv("PATH");
            std::string newPath = gpgDirStr + ";" + (existingPath ? existingPath : "");
            SetEnvironmentVariableA("PATH", newPath.c_str());
            _putenv(("PATH=" + newPath).c_str());
        }
#endif

        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

#ifdef __WINDOWS__
        // Set engine paths after gpgme_check_version (required by the GPGME API).
        // Create the homedir if it doesn't exist yet; gpg-agent will fail to start
        // if it can't find or write to the gnupg directory.
        System::mkdir_p(homedirPath);

        gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP,
                              gpgPath.empty() ? nullptr : gpgPath.string().c_str(),
                              homedirStr.c_str());
        if (!gpgPath.empty()) {
            auto gpgDir = gpgPath.parent_path();

            auto gpgconfPath = gpgDir / "gpgconf.exe";
            if (System::FileExists(gpgconfPath))
                gpgme_set_engine_info(GPGME_PROTOCOL_GPGCONF,
                                      gpgconfPath.string().c_str(), nullptr);

            auto connectPath = gpgDir / "gpg-connect-agent.exe";

            // gpg-agent writes its socket into <exe_dir>\gnupg\ (binary-relative on
            // MSYS2/Windows). Create it so the agent can start even on a fresh install.
            System::mkdir_p(gpgDir / "gnupg");

            // Ensure gpg-agent.conf has allow-loopback-pinentry.
            {
                auto agentConfPath = homedirPath / "gpg-agent.conf";
                std::string agentConf;
                System::TryReadFile(agentConfPath, agentConf);
                if (agentConf.find("allow-loopback-pinentry") == std::string::npos) {
                    if (!agentConf.empty() && agentConf.back() != '\n')
                        agentConf += '\n';
                    agentConf += "allow-loopback-pinentry\n";
                    System::TryWriteFile(agentConfPath, agentConf);
                    std::cerr << "[gpg-agent.conf] wrote allow-loopback-pinentry\n";
                }
            }

            // Ensure gpg.conf has pinentry-mode loopback so the agent uses loopback
            // when gpg is invoked directly (GPGME already passes --pinentry-mode=loopback
            // on the command line, but writing it to gpg.conf is the documented MSYS2 fix).
            {
                auto gpgConfPath = homedirPath / "gpg.conf";
                std::string gpgConf;
                System::TryReadFile(gpgConfPath, gpgConf);
                if (gpgConf.find("pinentry-mode loopback") == std::string::npos) {
                    if (!gpgConf.empty() && gpgConf.back() != '\n')
                        gpgConf += '\n';
                    gpgConf += "pinentry-mode loopback\n";
                    System::TryWriteFile(gpgConfPath, gpgConf);
                    std::cerr << "[gpg.conf] wrote pinentry-mode loopback\n";
                }
            }

            // Kill the running agent once per process run so it restarts fresh
            // and reads the updated gpg-agent.conf. The agent auto-starts on next use.
            if (System::FileExists(connectPath)) {
                static bool agentKilledThisRun = false;
                if (!agentKilledThisRun) {
                    agentKilledThisRun = true;
                    auto pw = System::ProcessWrapper();
                    pw.Init(connectPath.string(),
                            {"--homedir", homedirStr, "KILLAGENT", "/bye"},
                            gpgDir);
                    pw.Wait();
                    std::cerr << "[gpg-agent] killed for config reload\n";
                }
            }

            // Warm-start the agent (starts it if not running).
            if (System::FileExists(connectPath)) {
                auto pw = System::ProcessWrapper();
                pw.Init(connectPath.string(),
                        {"--homedir", homedirStr, "/bye"},
                        gpgDir);
                pw.Wait();
            }
        }

#ifdef __DEBUG__
        std::cerr << "[GPGME init] gpg=" << gpgPath.string()
                  << " home=" << homedirStr << "\n";
        {
            gpgme_engine_info_t ei = nullptr;
            gpgme_get_engine_info(&ei);
            for (auto e = ei; e; e = e->next)
                std::cerr << "[GPGME engine proto=" << e->protocol
                          << " bin=" << (e->file_name ? e->file_name : "null")
                          << " home=" << (e->home_dir ? e->home_dir : "(default)")
                          << " ver=" << (e->version ? e->version : "?") << "]\n";
        }
#endif
#endif

        return true;
    }

    std::filesystem::path GPG::GetGpgAgentConfPath() {
        return System::GetHomeFolder() / ".gnupg/gpg-agent.conf";
    }


    bool GPG::TryDecrypt(const std::filesystem::path &path, std::string &out) {
        if (!System::FileExists(path))
            RaiseClavisError(_(ERROR_NOT_A_FILE, path.string()));

        std::vector<uint8_t> data;
        if (!System::TryReadFile(path, data))
            return false;

        return TryDecrypt(data, out);
    }

    bool GPG::TryDecryptNoPrompt(const std::filesystem::path &path, std::string &out) {
        if (!System::FileExists(path))
            return false;

        std::vector<uint8_t> data;
        if (!System::TryReadFile(path, data))
            return false;

        return __TryDecryptData(data, out, false);
    }

    bool GPG::TryClearPassphraseCache() {
        InitializeGPGME();

        std::string executable = "gpg-connect-agent";
        std::vector<std::string> args;
        std::filesystem::path workingDir = ".";

#ifdef __WINDOWS__
        // There is no gpg-connect-agent on PATH in a bundled install, so it is looked up next to
        // whichever gpg binary GPGME was configured with, mirroring how the agent is warm-started
        // during initialisation.
        gpgme_engine_info_t info = nullptr;
        if (gpgme_get_engine_info(&info) == GPG_ERR_NO_ERROR) {
            for (auto engine = info; engine != nullptr; engine = engine->next) {
                if (engine->protocol != GPGME_PROTOCOL_OpenPGP || engine->file_name == nullptr)
                    continue;

                const auto gpgDir = std::filesystem::path(engine->file_name).parent_path();
                const auto connectPath = gpgDir / "gpg-connect-agent.exe";

                if (System::FileExists(connectPath)) {
                    executable = connectPath.string();
                    workingDir = gpgDir;

                    if (engine->home_dir != nullptr) {
                        args.emplace_back("--homedir");
                        args.emplace_back(engine->home_dir);
                    }
                }

                break;
            }
        }
#endif

        // reloadagent drops every cached passphrase without tearing the agent down, so anything
        // else already talking to it keeps working.
        args.emplace_back("reloadagent");
        args.emplace_back("/bye");

        try {
            auto pw = System::ProcessWrapper();
            pw.Init(executable, args, workingDir);
            pw.Wait();

            const int code = pw.GetExitCode();
            pw.Cleanup();

            return code == 0;
        } catch (...) {
            return false;
        }
    }

    bool GPG::TryDecrypt(const std::vector<uint8_t>& data, std::string& out) {
        return __TryDecryptData(data, out, true);
    }

    bool GPG::__TryDecryptData(const std::vector<uint8_t>& data, std::string& out, bool allowPrompt) {
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t cipher = nullptr;
        gpgme_data_t plain = nullptr;
        bool success = false;

        // Initialize GPGME
        InitializeGPGME();

        // Create a new context
        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR) {
            return false;
        }

        // Set the context to use ASCII armor if needed
        gpgme_set_armor(ctx, 0); // 0 for binary output, 1 for ASCII armor
        gpgme_set_pinentry_mode(ctx, allowPrompt ? GPGME_PINENTRY_MODE_ASK : GPGME_PINENTRY_MODE_CANCEL);

        // Create data objects from the input data
        err = gpgme_data_new_from_mem(&cipher, reinterpret_cast<const char*>(data.data()), data.size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
			std::cerr << std::string("error gpgme_data_new_from_mem: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_data_new(&plain);
        if (err != GPG_ERR_NO_ERROR) {
			std::cerr << std::string("error gpgme_data_new: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_data_release(cipher);
            gpgme_release(ctx);
            return false;
        }

        // Perform the decryption
        err = gpgme_op_decrypt(ctx, cipher, plain);
        if (err == GPG_ERR_NO_ERROR) {
            // Retrieve the decrypted data
            off_t size = gpgme_data_seek(plain, 0, SEEK_END);
            gpgme_data_seek(plain, 0, SEEK_SET);

            std::vector<char> buffer(size);
            ssize_t read_bytes = gpgme_data_read(plain, buffer.data(), buffer.size());
            if (read_bytes >= 0) {
                out.assign(buffer.data(), read_bytes);
                success = true;
            }
            System::SecureZero(buffer.data(), buffer.size());
        }

        // A failure in no-prompt mode just means the store is locked, which is expected and not
        // worth reporting.
        if (!success && allowPrompt)
			std::cerr << std::string("error gpgme_op_decrypt: ") + std::string(gpgme_strerror(err)) << "\n";

        // Clean up
        gpgme_data_release(plain);
        gpgme_data_release(cipher);
        gpgme_release(ctx);

        return success;
    }

    bool GPG::TryGetKeyFingerprint(const std::string& gpgid, std::string& outFingerprint) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_key_t key = nullptr;
        bool success = false;

        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        // Lookup the key by ID (can be full fingerprint, email, etc.)
        err = gpgme_get_key(ctx, gpgid.c_str(), &key, 0);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_get_key: ") + std::string(gpgme_strerror(err)) << "\n";
        } else if (key && key->subkeys && key->subkeys->fpr) {
            outFingerprint = key->subkeys->fpr;
            success = true;
        }

        // Cleanup
        gpgme_key_unref(key);
        gpgme_release(ctx);

        return success;
    }

    bool GPG::KeyExists(const std::string &gpgid) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_key_t key = nullptr;
        bool exists = false;

        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        err = gpgme_get_key(ctx, gpgid.c_str(), &key, 0);
        if (err == GPG_ERR_NO_ERROR && key != nullptr)
            exists = true;

        // Cleanup
        if (key)
            gpgme_key_unref(key);
        gpgme_release(ctx);

        return exists;
    }



    static gpgme_error_t symmetric_passphrase_cb(void *opaque, const char * /*uid_hint*/, const char * /*info*/, int prev_was_bad, int fd) {
        if (prev_was_bad)
            return GPG_ERR_CANCELED;

        const char* pw = static_cast<const char*>(opaque);
        size_t len = strlen(pw);

        #ifndef __WINDOWS__
        if (write(fd, pw, len) < 0) return GPG_ERR_CANCELED;
        if (write(fd, "\n", 1) < 0) return GPG_ERR_CANCELED;
        #else
        _write(fd, pw, (unsigned int)len);
        _write(fd, "\n", 1);
        #endif

        return GPG_ERR_NO_ERROR;
    }

    bool GPG::TryEncryptSymmetric(const std::string& passphrase, const std::vector<uint8_t>& plainData, std::vector<uint8_t>& out) {
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t plain = nullptr;
        gpgme_data_t cipher = nullptr;
        bool success = false;

        InitializeGPGME();

        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 0);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_passphrase_cb(ctx, symmetric_passphrase_cb, (void*)passphrase.c_str());

        err = gpgme_data_new_from_mem(&plain, reinterpret_cast<const char*>(plainData.data()), plainData.size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_data_new(&cipher);
        if (err != GPG_ERR_NO_ERROR) {
            gpgme_data_release(plain);
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_op_encrypt(ctx, nullptr, GPGME_ENCRYPT_SYMMETRIC, plain, cipher);
        if (err == GPG_ERR_NO_ERROR) {
            off_t size = gpgme_data_seek(cipher, 0, SEEK_END);
            gpgme_data_seek(cipher, 0, SEEK_SET);
            out.resize(size);
            ssize_t read_bytes = gpgme_data_read(cipher, out.data(), out.size());
            if (read_bytes >= 0) {
                out.resize(read_bytes);
                success = true;
            }
        }

        if (!success)
            std::cerr << "error TryEncryptSymmetric: " << gpgme_strerror(err) << "\n";

        gpgme_data_release(plain);
        gpgme_data_release(cipher);
        gpgme_release(ctx);

        return success;
    }

    bool GPG::TryDecryptSymmetric(const std::string& passphrase, const std::vector<uint8_t>& data, std::vector<uint8_t>& out) {
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t cipher = nullptr;
        gpgme_data_t plain = nullptr;
        bool success = false;

        InitializeGPGME();

        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 0);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        gpgme_set_passphrase_cb(ctx, symmetric_passphrase_cb, (void*)passphrase.c_str());

        err = gpgme_data_new_from_mem(&cipher, reinterpret_cast<const char*>(data.data()), data.size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_data_new(&plain);
        if (err != GPG_ERR_NO_ERROR) {
            gpgme_data_release(cipher);
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_op_decrypt(ctx, cipher, plain);
        if (err == GPG_ERR_NO_ERROR) {
            off_t size = gpgme_data_seek(plain, 0, SEEK_END);
            gpgme_data_seek(plain, 0, SEEK_SET);
            out.resize(size);
            ssize_t read_bytes = gpgme_data_read(plain, out.data(), out.size());
            if (read_bytes >= 0) {
                out.resize(read_bytes);
                success = true;
            }
        }

        if (!success)
            std::cerr << "error TryDecryptSymmetric: " << gpgme_strerror(err) << "\n";

        gpgme_data_release(plain);
        gpgme_data_release(cipher);
        gpgme_release(ctx);

        return success;
    }

    bool GPG::TryEncrypt(const std::string &data, std::vector<uint8_t> &out) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t plain = nullptr;
        gpgme_data_t cipher = nullptr;
        gpgme_key_t key[2] = {nullptr, nullptr};
        bool success = false;

        std::string id = PasswordStore::GetGPGID();  // your recipient's GPG ID

        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 0); // set to 1 for ASCII output if needed
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_ASK);


        // Look up the recipient's public key
        err = gpgme_get_key(ctx, id.c_str(), &key[0], 0);
        if (err != GPG_ERR_NO_ERROR) {
            gpgme_release(ctx);
            std::cerr << std::string("error gpgme_get_key: ") + std::string(gpgme_strerror(err)) << "\n";
            return false;
        }

        // Create data buffers
        err = gpgme_data_new_from_mem(&plain, data.c_str(), data.size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_data_new_from_mem: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_key_unref(key[0]);
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_data_new(&cipher);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_data_new: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_data_release(plain);
            gpgme_key_unref(key[0]);
            gpgme_release(ctx);
            return false;
        }

        // Encrypt
        err = gpgme_op_encrypt(ctx, key, GPGME_ENCRYPT_ALWAYS_TRUST, plain, cipher);
        if (err == GPG_ERR_NO_ERROR) {
            // Get encrypted output
            off_t size = gpgme_data_seek(cipher, 0, SEEK_END);
            gpgme_data_seek(cipher, 0, SEEK_SET);

            out.resize(size);
            ssize_t read_bytes = gpgme_data_read(cipher, out.data(), out.size());
            if (read_bytes >= 0) {
                out.resize(read_bytes); // trim if needed
                success = true;
            }
        }

        if (!success)
			std::cerr << std::string("error gpgme_op_encrypt: ") + std::string(gpgme_strerror(err)) << "\n";


        // Cleanup
        gpgme_data_release(plain);
        gpgme_data_release(cipher);
        gpgme_key_unref(key[0]);
        gpgme_release(ctx);

        return success;
    }

    bool GPG::TryExportKey(const std::string& gpgid, bool exportPrivate, std::vector<uint8_t>& out) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t keydata = nullptr;
        gpgme_key_t key = nullptr;
        bool success = false;

        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        // Set ASCII armor to get BEGIN PGP KEY BLOCK
        gpgme_set_armor(ctx, 1);
        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_ASK);

        // Get key (only need once, whether public or secret)
        err = gpgme_get_key(ctx, gpgid.c_str(), &key, exportPrivate ? 1 : 0);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_get_key: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_release(ctx);
            return false;
        }

        err = gpgme_data_new(&keydata);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_data_new: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_key_unref(key);
            gpgme_release(ctx);
            return false;
        }

        auto mode = exportPrivate? GPGME_EXPORT_MODE_SECRET : 0;

        // Always export public key
        gpgme_key_t keys[] = {key, nullptr};
        err = gpgme_op_export_keys(ctx, keys, mode, keydata);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_op_export_keys (public): ") + std::string(gpgme_strerror(err)) << "\n";
        } else {
            // Just public key export
            off_t size = gpgme_data_seek(keydata, 0, SEEK_END);
            gpgme_data_seek(keydata, 0, SEEK_SET);

            out.resize(size);
            ssize_t read_bytes = gpgme_data_read(keydata, out.data(), out.size());
            if (read_bytes >= 0) {
                out.resize(read_bytes);
                success = true;
            }
        }

        // Cleanup
        gpgme_data_release(keydata);
        gpgme_key_unref(key);
        gpgme_release(ctx);

        return success;
    }


    bool GPG::TryImportKey(const std::vector<uint8_t>& data, std::string& outFingerprint) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t keydata = nullptr;
        bool success = false;

        // Initialize GPGME
        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        // Create GPGME data object from memory
        err = gpgme_data_new_from_mem(&keydata, reinterpret_cast<const char*>(data.data()), data.size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_data_new_from_mem: ") + std::string(gpgme_strerror(err)) << "\n";
            gpgme_release(ctx);
            return false;
        }

        std::vector<std::string> importedFingerprints;

        // Perform the import
        err = gpgme_op_import(ctx, keydata);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_op_import: ") + std::string(gpgme_strerror(err)) << "\n";
        } else {
            success = true;
            // Extract fingerprints from the import result
            gpgme_import_result_t result = gpgme_op_import_result(ctx);
            for (gpgme_import_status_t status = result->imports; status != nullptr; status = status->next)
                if (status->fpr)
                    importedFingerprints.push_back(status->fpr);
        }

        // Cleanup
        gpgme_data_release(keydata);
        gpgme_release(ctx);

        bool didGetFingerprint = false;
        for (const auto& f : importedFingerprints) {
            if (!didGetFingerprint)
                outFingerprint = f;
            didGetFingerprint = true;

            #ifdef HAVE_GPGME_OP_SETOWNERTRUST
            if (!TryChangeKeyTrust(f, 5))
                success = false;
            #endif
        }


        return success;
    }


#ifdef HAVE_GPGME_OP_SETOWNERTRUST
    bool GPG::TryChangeKeyTrust(const std::string& fingerprint, int trustlevel) {
        std::string trustLevelString;
        switch (trustlevel) {
            case 1: trustLevelString = "undefined"; break;
            case 2: trustLevelString = "never"; break;
            case 3: trustLevelString = "marginal"; break;
            case 4: trustLevelString = "full"; break;
            case 5: trustLevelString = "ultimate"; break;
            default:
                RaiseClavisError(_(ERROR_INVALID_TRUST_LEVEL, std::to_string(trustlevel)));
        }

        gpgme_error_t err;
        gpgme_ctx_t ctx;

        // Initialize the GPGME library (required before any GPGME operation)
        InitializeGPGME();

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "Failed to create GPGME context: " << gpgme_strerror(err) << std::endl;
            return false;
        }

        // Retrieve the key by its fingerprint
        gpgme_key_t key;
        err = gpgme_get_key(ctx, fingerprint.c_str(), &key, 0); // 0 = public keyring only
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "Failed to retrieve key with fingerprint " << fingerprint << ": "
                      << gpgme_strerror(err) << std::endl;
            gpgme_release(ctx);
            return false;
        }

        // Start the owner trust operation
        err = gpgme_op_setownertrust(ctx, key, trustLevelString.c_str());
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "Failed to set owner trust for key: "
                      << gpgme_strerror(err) << std::endl;
            gpgme_key_unref(key); // Unreference the key to avoid memory leaks
            gpgme_release(ctx);
            return false;
        }

        // Clean up
        gpgme_key_unref(key); // Unreference the key
        gpgme_release(ctx);   // Release the GPGME context
        return true;
    }
#endif



    std::vector<GPG::Key> GPG::GetAllKeys() {
        std::vector<GPG::Key> result;

        // Initialize GPGME
        InitializeGPGME();

        gpgme_ctx_t ctx;
        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "GPGME context error: " << gpgme_strerror(err) << "\n";
            return result;
        }

        // Start key listing (false = list public keys)
        err = gpgme_op_keylist_start(ctx, nullptr, true);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "Keylist start error: " << gpgme_strerror(err) << "\n";
            gpgme_release(ctx);
            return result;
        }

        gpgme_key_t key;
        while ((err = gpgme_op_keylist_next(ctx, &key)) == GPG_ERR_NO_ERROR) {
            for (gpgme_user_id_t uid = key->uids; uid != nullptr; uid = uid->next) {
                GPG::Key k;

                if (uid->name)
                    k.username = uid->name;
                if (uid->email)
                    k.keyname = uid->email;
                if (uid->comment)
                    k.comment = uid->comment;
                if (key->fpr)
                    k.fingerprint = key->fpr;

                result.push_back(k);
            }

            gpgme_key_unref(key);
        }

        if (err != GPG_ERR_EOF && err != 117456895) {
            std::cerr << "Keylist iteration error: " << gpgme_strerror(err) << "\n";
        }

        gpgme_op_keylist_end(ctx);
        gpgme_release(ctx);

        return result;

    }

    std::string GPG::KeyToString(const Key &key, bool escapeChars) {
        std::string ret;

        ret += key.username;

        if (! key.comment.empty())
            ret += " (" + key.comment + ")";

        std::string openBracket = escapeChars? " &lt;" : " <";
        std::string closeBracket = escapeChars? "&gt;" : ">";


        ret += openBracket + key.keyname + closeBracket;

        return ret;
    }

    std::string GPG::KeyToStringFull(const Key &key) {
        std::string ret = KeyToString(key, false);
        ret += "\nFingerpring: " + key.fingerprint + "\n";

        return ret;
    }


    std::string GPG::KeyTypeToString(KeyType type) {
        switch (type) {
            case KeyType::RSA_DSA:
                return "RSA + DSA";
            case KeyType::DSA_ELGAMAL:
                return "DSA + Elgamal";
            case KeyType::ECC_25519:
                return "ECC Curve 25519 (" + _(MISC_RECOMMENDED) + ")";
            case KeyType::ECC_NIST_P256:
                return "ECC NIST P-256";
            case KeyType::ECC_NIST_P384:
                return "ECC NIST P-384";
            case KeyType::ECC_NIST_P521:
                return "ECC NIST P-521";
            case KeyType::ECC_BRAINPOOL_P256:
                return "ECC Brainpool P-256";

            default:
                RaiseClavisError(_(ERROR_INVALID_KEY_TYPE, std::to_string(static_cast<int>(type))));
        }
    }

    std::string GPG::KeyTypeToStringCode(KeyType type) {
        int v = static_cast<int>(type);
        return std::to_string(v);
    }

    GPG::KeyType GPG::StringCodeToKeyType(const std::string &str) {
        try {
            int v = std::stoi(str);
            return KeyType(v);
        } catch (...) {
            RaiseClavisError(_(UNABLE_TO_PARSE_KEYTYPE_CODE, str));
        }
    }

    std::vector<GPG::KeyType> GPG::GetAllKeyTypes() {
        return {
            KeyType::ECC_25519,
            KeyType::RSA_DSA,
            KeyType::DSA_ELGAMAL,
            KeyType::ECC_NIST_P256,
            KeyType::ECC_NIST_P384,
            KeyType::ECC_NIST_P521,
            KeyType::ECC_BRAINPOOL_P256,
        };
    }

    GPG::KeySizeRange GPG::GetKeySizeRange(const KeyType &type) {
        KeySizeRange range;

        switch (type) {
            case KeyType::ECC_25519:
            case KeyType::ECC_BRAINPOOL_P256:
            case KeyType::ECC_NIST_P256:
            case KeyType::ECC_NIST_P384:
            case KeyType::ECC_NIST_P521:
                range.min = -1; range.max = -1; range.def = -1;
                return range;

            case KeyType::RSA_DSA:
                range.min = 1024; range.max = 4096; range.def = 3072;
                return range;

            case KeyType::DSA_ELGAMAL:
                range.min = 1024; range.max = 3072; range.def = 2048;
                return range;

            default:
                RaiseClavisError(_(ERROR_INVALID_KEY_TYPE, std::to_string(static_cast<int>(type))));
        }
    }

    std::string GPG::GetKeyParams(Key data) {
        std::ostringstream params;
        params.imbue(std::locale::classic());

        if (data.type == KeyType::RSA_DSA || data.type == KeyType::DSA_ELGAMAL) {
            std::string type;

            if (data.type == KeyType::RSA_DSA)
                type = "RSA";
            else
                type = "DSA";

            params << "Key-Type: " << type << "\n";
            params << "Key-Length: " << data.length << "\n";

            return params.str();
        }

        std::string type;
        std::string curve;
        std::string subtype;
        std::string subcurve;

        if (data.type == KeyType::ECC_25519) {
            type = "EDDSA";
            curve = "ed25519";
            subtype = "ECDH";
            subcurve = "cv25519";
        }

        if (data.type == KeyType::ECC_NIST_P256 ||
            data.type == KeyType::ECC_NIST_P384 ||
            data.type == KeyType::ECC_NIST_P521)
        {
            type = "ECDSA";
            subtype = "ECDH";
            curve = "nistp";

            if (data.type == KeyType::ECC_NIST_P256)
                curve += "256";
            if (data.type == KeyType::ECC_NIST_P384)
                curve += "384";
            if (data.type == KeyType::ECC_NIST_P521)
                curve += "521";

            subcurve = curve;
        }

        if (data.type == KeyType::ECC_BRAINPOOL_P256) {
            type = "ECDSA";
            curve = "brainpoolP256r1";
            subtype = "ECDH";
            subcurve = "brainpoolP256r1";
        }

        params << "Key-Type: " << type << "\n";
        params << "Key-Curve: " << curve << "\n";
        params << "Subkey-Type: " << subtype << "\n";
        params << "Subkey-Curve: " << subcurve << "\n";

        return params.str();
    }


    bool GPG::TryCreateKey(const Key& data, std::string& outFingerprint) {
        InitializeGPGME();

        gpgme_ctx_t ctx = nullptr;
        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 1);

#ifdef __WINDOWS__
        // gpgme_op_createkey requires the loopback pinentry path, which is unreliable
        // with MSYS2's gpg-agent on Windows (passphrase callback is never invoked).
        // Use gpgme_op_genkey with the passphrase embedded in the batch parameter block
        // to bypass pinentry entirely while still protecting the key material.
        {
            std::ostringstream parms;
            parms << "<GnupgKeyParms format=\"internal\">\n";

            switch (data.type) {
                case KeyType::ECC_25519:
                    parms << "Key-Type: eddsa\nKey-Curve: ed25519\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: cv25519\n";
                    break;
                case KeyType::RSA_DSA:
                    parms << "Key-Type: rsa\nKey-Length: " << data.length << "\nKey-Usage: sign\n"
                          << "Subkey-Type: rsa\nSubkey-Length: " << data.length << "\nSubkey-Usage: encrypt\n";
                    break;
                case KeyType::DSA_ELGAMAL:
                    parms << "Key-Type: dsa\nKey-Length: " << data.length << "\nKey-Usage: sign\n"
                          << "Subkey-Type: elg\nSubkey-Length: " << data.length << "\nSubkey-Usage: encrypt\n";
                    break;
                case KeyType::ECC_NIST_P256:
                    parms << "Key-Type: ecdsa\nKey-Curve: nistp256\nKey-Usage: sign\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: nistp256\nSubkey-Usage: encrypt\n";
                    break;
                case KeyType::ECC_NIST_P384:
                    parms << "Key-Type: ecdsa\nKey-Curve: nistp384\nKey-Usage: sign\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: nistp384\nSubkey-Usage: encrypt\n";
                    break;
                case KeyType::ECC_NIST_P521:
                    parms << "Key-Type: ecdsa\nKey-Curve: nistp521\nKey-Usage: sign\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: nistp521\nSubkey-Usage: encrypt\n";
                    break;
                case KeyType::ECC_BRAINPOOL_P256:
                    parms << "Key-Type: ecdsa\nKey-Curve: brainpoolP256r1\nKey-Usage: sign\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: brainpoolP256r1\nSubkey-Usage: encrypt\n";
                    break;
                default:
                    parms << "Key-Type: eddsa\nKey-Curve: ed25519\n"
                          << "Subkey-Type: ecdh\nSubkey-Curve: cv25519\n";
                    break;
            }

            parms << "Name-Real: " << data.username << "\n";
            parms << "Name-Email: " << data.keyname << "\n";
            if (!data.comment.empty())
                parms << "Name-Comment: " << data.comment << "\n";
            parms << "Expire-Date: 0\n";

            if (data.password.empty())
                parms << "%no-protection\n";
            else
                parms << "Passphrase: " << data.password << "\n";

            parms << "</GnupgKeyParms>\n";
            std::string parmsStr = parms.str();

            std::cerr << "[TryCreateKey] using gpgme_op_genkey (Windows batch mode)\n";

            err = gpgme_op_genkey(ctx, parmsStr.c_str(), nullptr, nullptr);
            if (err != GPG_ERR_NO_ERROR) {
                std::cerr << "error gpgme_op_genkey: " << gpgme_strerror(err)
                          << " [src=" << gpgme_strsource(err)
                          << " code=" << gpgme_err_code(err) << "]\n";
                gpgme_release(ctx);
                return false;
            }

            gpgme_genkey_result_t result = gpgme_op_genkey_result(ctx);
            if (result && result->fpr)
                outFingerprint = result->fpr;

            gpgme_release(ctx);
            return !outFingerprint.empty();
        }
#endif

        gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
        if (!data.password.empty())
            gpgme_set_passphrase_cb(ctx, symmetric_passphrase_cb, (void*)data.password.c_str());

        // Build user ID string: "Real Name (comment) <email>"
        std::string userid = data.username;
        if (!data.comment.empty())
            userid += " (" + data.comment + ")";
        userid += " <" + data.keyname + ">";

        // Map KeyType to gpg --quick-gen-key algo strings.
        // "default" creates ed25519+cv25519 (primary sign/cert + subkey encr).
        // For other types, we create the primary key and then add an encryption subkey.
        std::string primaryAlgo;
        std::string subkeyAlgo;
        switch (data.type) {
            case KeyType::ECC_25519:
                primaryAlgo = "default";
                break;
            case KeyType::RSA_DSA:
                primaryAlgo = "rsa" + std::to_string(data.length);
                subkeyAlgo  = "rsa" + std::to_string(data.length);
                break;
            case KeyType::DSA_ELGAMAL:
                primaryAlgo = "dsa" + std::to_string(data.length);
                subkeyAlgo  = "elg" + std::to_string(data.length);
                break;
            case KeyType::ECC_NIST_P256:
                primaryAlgo = "nistp256";
                subkeyAlgo  = "nistp256";
                break;
            case KeyType::ECC_NIST_P384:
                primaryAlgo = "nistp384";
                subkeyAlgo  = "nistp384";
                break;
            case KeyType::ECC_NIST_P521:
                primaryAlgo = "nistp521";
                subkeyAlgo  = "nistp521";
                break;
            case KeyType::ECC_BRAINPOOL_P256:
                primaryAlgo = "brainpoolP256r1";
                subkeyAlgo  = "brainpoolP256r1";
                break;
            default:
                primaryAlgo = "default";
                break;
        }

        unsigned int flags = GPGME_CREATE_NOEXPIRE;
        if (data.password.empty())
            flags |= GPGME_CREATE_NOPASSWD;

        err = gpgme_op_createkey(ctx, userid.c_str(), primaryAlgo.c_str(), 0, 0, nullptr, flags);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "error gpgme_op_createkey: " << gpgme_strerror(err)
                      << " [src=" << gpgme_strsource(err)
                      << " code=" << gpgme_err_code(err) << "]\n";
            gpgme_release(ctx);
            return false;
        }

        gpgme_genkey_result_t result = gpgme_op_genkey_result(ctx);
        if (result && result->fpr)
            outFingerprint = result->fpr;

        // For key types that need a separate encryption subkey.
        if (!subkeyAlgo.empty() && !outFingerprint.empty()) {
            gpgme_key_t key = nullptr;
            err = gpgme_get_key(ctx, outFingerprint.c_str(), &key, 1);
            if (err == GPG_ERR_NO_ERROR && key) {
                unsigned int subflags = GPGME_CREATE_NOEXPIRE | GPGME_CREATE_ENCR;
                if (data.password.empty())
                    subflags |= GPGME_CREATE_NOPASSWD;
                err = gpgme_op_createsubkey(ctx, key, subkeyAlgo.c_str(), 0, 0, subflags);
                if (err != GPG_ERR_NO_ERROR)
                    std::cerr << "error gpgme_op_createsubkey: " << gpgme_strerror(err) << "\n";
                gpgme_key_unref(key);
            }
        }

        gpgme_release(ctx);
        return !outFingerprint.empty();
    }






}
