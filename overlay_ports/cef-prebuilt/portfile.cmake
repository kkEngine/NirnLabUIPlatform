#set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_110.0.30+g3c2b68f+chromium-110.0.5481.178_windows64.tar.bz2")
#
#vcpkg_download_distfile(ARCHIVE
#    URLS ${CEF_URL}
#    FILENAME "cef.tar.bz2"
#    SHA512 7384d386625709e40002e21b3b460cd6fafebaebec1abb4198b05ee14a046fbc5d23f5763f75bb388474175b8c263cacc82e2ed249f935ec28da64b55acb5d07
#)

set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_108.4.13+ga98cd4c+chromium-108.0.5359.125_windows64.tar.bz2")

vcpkg_download_distfile(ARCHIVE
    URLS ${CEF_URL}
    FILENAME "cef.tar.bz2"
    SHA512 a11f1a266ba35d86ff272f5c71ba553cdf3b30aa0478556cb7973e644716bb0bfa7f2b3a57b78e573ec2e6132f48b36fd8189978ecb548876e4dc8c8c3aad59f
)

vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE ${ARCHIVE}
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${SOURCE_PATH})

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/include" DESTINATION "${CURRENT_PACKAGES_DIR}")
file(INSTALL "${SOURCE_PATH}/Debug" "${SOURCE_PATH}/Release" "${SOURCE_PATH}/Resources" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
