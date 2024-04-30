function(set_compile_options target)
    target_compile_features(
        ${target}
        PRIVATE
            cxx_std_20
    )

    target_compile_options(
        ${target}
        PRIVATE
            "/sdl"      # Enable Additional Security Checks
            "/utf-8"    # Set Source and Executable character sets to UTF-8
            "/Zi"       # Debug Information Format

            "/permissive-"          # Standards conformance
            "/MP"

            "/Zc:alignedNew"        # C++17 over-aligned allocation
            "/Zc:__cplusplus"       # Enable updated __cplusplus macro
            "/Zc:externConstexpr"   # Enable extern constexpr variables
            "/Zc:preprocessor"      # Enable preprocessor conformance mode

            "/wd4200" # nonstandard extension used : zero-sized array in struct/union
            "/wd4100" # unreferenced formal parameter
            #"/WX-" # treats warning as error OFF

            "$<$<CONFIG:DEBUG>:>"
            "$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3;>"
    )

    if(MSVC)
        set_target_properties(
            ${target}
            PROPERTIES
            MSVC_RUNTIME_LIBRARY
                "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        )

        target_link_options(
            ${target}
            PRIVATE
                "$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF;/DEBUG:FULL;>"
                "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;>"
        )

        target_compile_definitions(
	        ${target}
	        PUBLIC
                SKYRIM_SUPPORT_AE
        )
    endif()
endfunction()

function(set_external_project_options target)
    set_target_properties(
        ${target}
        PROPERTIES FOLDER
            ${PROJECT_FOLDER_EXTERNAL}
    )

    if(MSVC)
        set_target_properties(
            ${target}
            PROPERTIES MSVC_RUNTIME_LIBRARY 
                "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        )
    endif()

    set_target_properties(
      ${target}
      PROPERTIES CMAKE_CXX_FLAGS
          "/EHsc /MP /W4 /WX /external:W0"
    )

    if (WIN32)
        target_compile_definitions(
          ${target}
          PRIVATE
            WIN32
        )
    endif()
endfunction()
