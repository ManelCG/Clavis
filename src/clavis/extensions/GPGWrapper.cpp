#include <extensions/GPGWrapper.h>

#include <iostream>
#include <gpgme.h>

#include <error/ClavisError.h>
#include <system/Extensions.h>

#include <language/Language.h>

namespace Clavis {
    bool GPG::TryDecrypt(const std::filesystem::path &path, std::string &out) {
        if (!System::FileExists(path))
            RaiseClavisError(_(ERROR_NOT_A_FILE, path.string()));

        std::vector<uint8_t> data;
        if (!System::TryReadFile(path, data))
            return false;

        return TryDecrypt(data, out);
    }

    bool GPG::TryDecrypt(const std::vector<uint8_t>& data, std::string& out) {
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t cipher = nullptr;
        gpgme_data_t plain = nullptr;
        bool success = false;

        // Initialize GPGME
        gpgme_check_version(nullptr);

        // Create a new context
        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR) {
            return false;
        }

        // Set the context to use ASCII armor if needed
        gpgme_set_armor(ctx, 0); // 0 for binary output, 1 for ASCII armor

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
        }

        if (!success)
			std::cerr << std::string("error gpgme_op_decrypt: ") + std::string(gpgme_strerror(err)) << "\n";

        // Clean up
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

        std::string id = GetGPGID();  // your recipient's GPG ID

        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 0); // set to 1 for ASCII output if needed


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

        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

        err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        // Set ASCII armor to get BEGIN PGP KEY BLOCK
        gpgme_set_armor(ctx, 1);

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

    bool GPG::TryImportKey(const std::vector<uint8_t>& data) {
        gpgme_error_t err;
        gpgme_ctx_t ctx = nullptr;
        gpgme_data_t keydata = nullptr;
        bool success = false;

        // Initialize GPGME
        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

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

        // Perform the import
        err = gpgme_op_import(ctx, keydata);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << std::string("error gpgme_op_import: ") + std::string(gpgme_strerror(err)) << "\n";
        } else {
            success = true;
        }

        // Cleanup
        gpgme_data_release(keydata);
        gpgme_release(ctx);

        return success;
    }



    std::string GPG::GetGPGID() {
        auto p = System::GetGPGIDPath();
        if (!System::FileExists(p))
            RaiseClavisError(_(ERROR_GPG_ID_FILE_NOT_FOUND, p.string()));

        std::string id;
        if (!System::TryReadFile(p, id))
            RaiseClavisError(_(ERROR_CANNOT_READ_GPGID_FILE, p.string()));

        id.erase(id.find_last_not_of(" \r\n\t") + 1);

        return id;
    }

    std::vector<GPG::Key> GPG::GetAllKeys() {
        std::vector<GPG::Key> result;

        // Initialize GPGME
        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

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


    bool GPG::TryCreateKey(const Key& data) {
        gpgme_check_version(nullptr);
        gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

        gpgme_ctx_t ctx = nullptr;
        gpgme_error_t err = gpgme_new(&ctx);
        if (err != GPG_ERR_NO_ERROR)
            return false;

        gpgme_set_armor(ctx, 1); // Make output ASCII armored

        // Create parameter string
        std::ostringstream params;
        params << "<GnupgKeyParms format=\"internal\">\n";

        params << GetKeyParams(data);

        params << "Name-Real: " << data.username << "\n";
        params << "Name-Comment: " << data.comment << "\n";
        params << "Name-Email: " << data.keyname << "\n";

        if (!data.password.empty())
            params << "Passphrase: " << data.password << "\n";
        else
            params << "Passphrase: \n"; // empty password

        params << "Expire-Date: 0\n"; // Never expires
        params << "</GnupgKeyParms>\n";

        gpgme_data_t key_params;
        err = gpgme_data_new_from_mem(&key_params, params.str().c_str(), params.str().size(), 0);
        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "error gpgme_data_new_from_mem: " << gpgme_strerror(err) << "\n";
            gpgme_release(ctx);
            return false;
        }

        auto paramsStr = params.str();
        auto cstr = paramsStr.c_str();
        // Generate the key
        err = gpgme_op_genkey(ctx,cstr, nullptr, nullptr);
        gpgme_data_release(key_params);

        if (err != GPG_ERR_NO_ERROR) {
            std::cerr << "error gpgme_op_genkey: " << gpgme_strerror(err) << "\n";
            gpgme_release(ctx);
            return false;
        }

        // Optional: get generated key info
        gpgme_genkey_result_t result = gpgme_op_genkey_result(ctx);
        if (result && result->fpr) {
            std::cout << "Key generated with fingerprint: " << result->fpr << "\n";
        }

        gpgme_release(ctx);
        return true;
    }






}
