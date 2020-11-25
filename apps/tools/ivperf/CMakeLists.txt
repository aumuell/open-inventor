set(BARCHART_DIR ../../nodes/BarChart)

add_executable(ivperf
	ivperf.c++
        ${BARCHART_DIR}/BarChart.c++
)

target_include_directories(ivperf PRIVATE ${BARCHART_DIR})

target_link_libraries(ivperf PRIVATE Inventor InventorXt)

install(TARGETS ivperf)
