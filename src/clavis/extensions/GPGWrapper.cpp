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



}
