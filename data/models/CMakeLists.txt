install(DIRECTORY .
    DESTINATION ${IVDATADIR}
    FILES_MATCHING
    PATTERN "*.env"
    PATTERN "*.iv"
    PATTERN "README"
)

set(CHESS ${CMAKE_INSTALL_PREFIX}/${IVDATADIR}/scenes/chesschairs.iv)
install(CODE
    "
    message(\"Transforming to binary: ${CHESS}\")
    execute_process(
        COMMAND ivcat -b ${CMAKE_CURRENT_SOURCE_DIR}/scenes/chesschairs.iv.asc
        OUTPUT_FILE ${CHESS}
    )
    "
)
