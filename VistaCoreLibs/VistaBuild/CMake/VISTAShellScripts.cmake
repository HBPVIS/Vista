function(CREATE_SHELLSCRIPT FILE_OUT APPCODE)
# TODO Support for Windows
    configure_file(
        ${VISTA_DIR}/vista_set_paths.sh_proto
        ${FILE_OUT}
    )
endfunction(CREATE_SHELLSCRIPT)
