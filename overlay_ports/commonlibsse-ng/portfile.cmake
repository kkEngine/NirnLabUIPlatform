# kkEngine fork: Skyrim SE 1.5.97 or AE 1.7.99+, one runtime per build (feature "se" selects SE).
# The commit is in https://github.com/kkEngine/CommonLibSSE-NG through PR #3, not on a branch yet.
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO kkEngine/CommonLibSSE-NG
    REF e45cc3cb9fc539cdb9b06be58c50f4f6cdf33e94
    SHA512 9a87b804887a7e3142f31a7a41560d1b6367ce895264e894d8d00c0f148ea8496d4550fb140acf93da22c9bcad7650a1efce872b57694374d654968dbd2a803b
    HEAD_REF main
)

if("se" IN_LIST FEATURES)
    set(SKYRIM_TARGET -DENABLE_SKYRIM_SE=on -DENABLE_SKYRIM_AE=off)
else()
    set(SKYRIM_TARGET -DENABLE_SKYRIM_SE=off -DENABLE_SKYRIM_AE=on)
endif()

vcpkg_configure_cmake(
        SOURCE_PATH "${SOURCE_PATH}"
        PREFER_NINJA
        OPTIONS -DBUILD_TESTS=off -DSKSE_SUPPORT_XBYAK=on ${SKYRIM_TARGET}
)

vcpkg_install_cmake()
vcpkg_cmake_config_fixup(PACKAGE_NAME CommonLibSSE CONFIG_PATH lib/cmake)
vcpkg_copy_pdbs()

file(GLOB CMAKE_CONFIGS "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE/CommonLibSSE/*.cmake")
file(INSTALL ${CMAKE_CONFIGS} DESTINATION "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE")
file(INSTALL "${SOURCE_PATH}/cmake/CommonLibSSE.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE/CommonLibSSE")

file(
        INSTALL "${SOURCE_PATH}/LICENSE"
        DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
        RENAME copyright)
