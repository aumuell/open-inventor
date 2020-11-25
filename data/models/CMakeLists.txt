add_custom_target(chess ALL
    COMMAND ivcat -b ${CMAKE_CURRENT_SOURCE_DIR}/scenes/chesschairs.iv.asc > scenes/chesschairs.iv
    DEPENDS scenes/chesschairs.iv.asc
    BYPRODUCTS scenes/chesschairs.iv
)

install(DIRECTORY .
    DESTINATION ${IVDATADIR}
    FILES_MATCHING
    PATTERN "*.env"
    PATTERN "*.iv"
    PATTERN "README"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/scenes/chesschairs.iv
    DESTINATION ${IVDATADIR}/scenes
)
