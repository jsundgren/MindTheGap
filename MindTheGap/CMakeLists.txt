# Add as a subdirectory SimpleRenderEngineProject using add_subdirectory
IF (NOT DEFINED all_libs)
    message( FATAL_ERROR "This CMakeLists.txt must be included using the command add_subdirectory(name)" )
ENDIF (NOT DEFINED all_libs)
# Ensure SimpleRenderEngineProject is up-to-date
IF (WIN32)
    IF (NOT DEFINED DLLFileList)
        message( FATAL_ERROR "The SimpleRenderEngineProject has been updated. Clone the project (https://github.com/mortennobel/SimpleRenderEngineProject) again the project again and start over." )
    ENDIF(NOT DEFINED DLLFileList)
ENDIF(WIN32)

# Add hpp and cpp files to Snake variable
file(GLOB MindTheGap
        "*.hpp"
        "*.h"
        "*.cpp"
        )

# Compile cpp files (from Snake variable)
add_executable(MindTheGap ${MindTheGap})
target_link_libraries(MindTheGap ${all_libs})

# copy files to dest
file(COPY GUI_SpriteSheet.json GUI_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY GUI_SpriteSheet.json GUI_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
# Character SpriteSheet
file(COPY OPP_Sheet.png OPP_Sheet.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY OPP_Sheet.png OPP_Sheet.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

# Map 1
file(COPY Map_1_Background.png Map_1.json Map_1_SpriteSheet.json Map_1_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY Map_1_Background.png Map_1.json Map_1_SpriteSheet.json Map_1_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
# Map 2
file(COPY Map_2.json Map_2_SpriteSheet.json Map_2_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY Map_2.json Map_2_SpriteSheet.json Map_2_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
# Map 3
file(COPY Map_3.json Map_3_SpriteSheet.json Map_3_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY Map_3.json Map_3_SpriteSheet.json Map_3_SpriteSheet.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

# font
file(COPY pusab.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY pusab.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

IF (WIN32)
    # Copy dll files to ${CMAKE_CURRENT_BINARY_DIR}/Debug
    file(COPY ${DLLFileList} DESTINATION Debug)
    file(COPY ${DLLFileList} DESTINATION Release)

    # Set working directory to ${CMAKE_CURRENT_BINARY_DIR}/Debug
    set_target_properties(MindTheGap PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug)
ENDIF(WIN32)
