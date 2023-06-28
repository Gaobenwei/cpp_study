cmake_minimum_required(VERSION 3.5)

project(CppDesignPatterns)
#set命令定义了一个变量PATTERNS，其中包含了设计模式的名称列表。

set(PATTERNS
    abstract-factory
    adapter
    bridge
    builder
    chain-of-responsibility
    command
    composite
    decorator
    facade
    factory-method
    flyweight
    interpreter
    iterator
    mediator
    memento
    observer
    prototype
    proxy
    singleton
    state
    strategy
    template-method
    visitor
)

#foreach循环迭代PATTERNS变量中的每个项目。
#对于每个项目，它使用file命令和GLOB选项来获取与模式名称相对应的.cpp文件列表，并将其存储在_files变量中。
#然后，使用message命令输出一个带有模式名称的状态消息。



#内部的foreach循环迭代_files变量中的每个.cpp文件。
#对于每个文件，它使用get_filename_component命令获取文件名并将其存储在_file_name变量中。
#然后，它将_project_name设置为文件名，并使用message命令输出一个带有文件路径的状态消息。
#最后，它使用add_executable命令将文件添加到构建系统中，将文件路径作为参数，
#并使用_project_name作为可执行文件的名称。
foreach(_dir IN ITEMS ${PATTERNS})
    file(GLOB _files "${_dir}/*.cpp")
    message(STATUS "Pattern `${_dir}':")

    foreach(_file IN ITEMS ${_files})

        get_filename_component(_file_name
                                ${_file} NAME
        )

        set(_project_name "${_file_name}")
        message(STATUS "  ${_dir}/${_file_name} is going to be built")
        
        add_executable(${_project_name} "${_dir}/${_file_name}")
    endforeach()
    
endforeach()
