#pragma once

#include <image/icons.h>
#include <password_store/PasswordStoreElement.h>

namespace Clavis::GUI {
    // Single source of truth for "what does this kind of store element look like". The folderview
    // and the Edit Workspace dialog both draw the same items, so they must not each carry their
    // own copy of these rules.
    inline Icons::IconDefinition ResolveIconForElement(
        const PasswordStoreElements::PasswordStoreElement& element)
    {
        if (element.IsWorkspace())
            return Icons::Actions::Build;

        if (element.IsFolder())
            return Icons::Actions::Folder;

        if (element.IsGPGFile())
            return Icons::Actions::Password;

        if (element.IsTwoFactorFile())
            return Icons::TwoFactor;

        return Icons::Actions::Document;
    }
}
