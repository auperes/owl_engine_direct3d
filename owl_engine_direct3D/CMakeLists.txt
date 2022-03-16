set(HEADER_FILES
    application.h
    dxerr.h
    DXGetErrorDescription.inl
    DXGetErrorString.inl
    DXTrace.inl
    DxgiInfoManager.h
    exception.h
    graphics.h
    keyboard.h
    lean_window.h
    mouse.h
    owl_window.h
    resource.h
    timer.h
    windows_message_map.h)

set(SOURCE_FILES
    application.cpp
    dxerr.cpp
    DxgiInfoManager.cpp
    exception.cpp
    graphics.cpp
    keyboard.cpp
    mouse.cpp
    owl_window.cpp
    timer.cpp
    windows_message_map.cpp
    owl_engine_direct3d.rc
    main.cpp)

set(SHADER_FILES
    pixel_passthrough.hlsl
    vertex_passthrough.hlsl)

add_custom_target(shaders)

set_source_files_properties(vertex_passthrough.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(pixel_passthrough.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${SHADER_FILES} PROPERTIES ShaderModel "4_0")

foreach(FILE ${SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  get_source_file_property(shadertype ${FILE} ShaderType)
  get_source_file_property(shadermodel ${FILE} ShaderModel)
  add_custom_command(TARGET shaders
                     COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<$<CONFIG:DEBUG>:/Od> /Zi /Fo ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${FILE_WE}.pdb ${FILE}
                     MAIN_DEPENDENCY ${FILE}
                     COMMENT "HLSL ${FILE}"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     VERBATIM)
endforeach(FILE)

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${HEADER_FILES})
add_dependencies(${PROJECT_NAME} shaders)

set(LIBS d3d11 D3DCompiler)
target_link_libraries(${PROJECT_NAME} ${LIBS})
