###############################################################################
# This file defines the macros that ParaView-based clients can use of creating
# custom ParaView client builds with custom branding and configuration.
# 
# build_paraview_client(
#   # The name for this client. This is the name used for the executable created.
#   paraview
#
#   # Optional name for the application (If none is specified then the
#   # client-name is used.
#   APPLICATION_NAME "ParaView"
# 
#   # This is the title bar text. If none is provided the name will be used.
#   TITLE "Kitware ParaView"
#   
#   # This is the organization name.
#   ORGANIZATION "Kitware Inc."
# 
#   # PNG Image to be used for the Splash screen. If none is provided, default
#   # ParaView splash screen will be shown. 
#   SPLASH_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/Splash.png"
# 
#   # Provide version information for the client.
#   VERSION_MAJOR ${PARAVIEW_VERSION_MAJOR}
#   VERSION_MINOR ${PARAVIEW_VERSION_MINOR}
#   VERSION_PATCH ${PARAVIEW_VERSION_PATCH}
# 
#   # Icon to be used for the Mac bundle.
#   BUNDLE_ICON   "${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns"
# 
#   # Icon to be used for the Windows application.
#   APPLICATION_ICON "${CMAKE_CURRENT_SOURCE_DIR}/Icon.ico"
#
#   # Name of the class to use for the main window. If none is specified,
#   # default QMainWindow will be used.
#   PVMAIN_WINDOW QMainWindow-subclass
#   PVMAIN_WINDOW_INCLUDE QMainWindow-subclass-header
# 
#   # Next specify the plugins that are needed to be built and loaded on startup
#   # for this client to work. These must be specified in the order that they
#   # should be loaded. The name is the name of the plugin specified in the
#   # add_paraview_plugin call.
#   # Currently, only client-based plugins are supported. i.e. no effort is made
#   # to load the plugins on the server side when a new server connection is made.
#   # That may be added in future, if deemed necessary.
#   REQUIRED_PLUGINS PointSpritePlugin
# 
#   # Next specify the plugin that are not required, but if available, should be
#   # loaded on startup. These must be specified in the order that they
#   # should be loaded. The name is the name of the plugin specified in the
#   # add_paraview_plugin call.
#   # Currently, only client-based plugins are supported. i.e. no effort is made
#   # to load the plugins on the server side when a new server connection is made.
#   # That may be added in future, if deemed necessary.
#   OPTIONAL_PLUGINS ClientGraphView ClientTreeView
#
#   # Extra targets that this executable depends on. Useful only if you are
#   # building extra libraries for your application.
#   EXTRA_DEPENDENCIES blah1 blah2
#
#   # GUI Configuration XMLs that are used to configure the client eg. readers,
#   # writers, sources menu, filters menu etc.
#   GUI_CONFIGURATION_XMLS <list of xml files>
#
#   # The Qt compressed help file (*.qch) which provides the documentation for the
#   # application. *.qch files are typically generated from *.qhp files using
#   # the qhelpgenerator executable.
#   COMPRESSED_HELP_FILE MyApp.qch
#
#   # Additional source files.
#   SOURCES <list of source files>
#
#   # If this option is present, then this macro will create a library named
#   # pq{Name}Initializer with all the source components generated by this macro
#   # that the executable links against. Otherwise, for sake of simplicity no
#   # extra library is created.
#   MAKE_INITIALIZER_LIBRARY
#   
#   # Optional to specify the installation prefix for all the binaries.
#   # "bin" is used if none is specified.
#   INSTALL_BIN_DIR "bin"
#
#   # Optional to specify the installation prefix for all the libraries.
#   # "lib/appname-major.minor" is used if none is specified (on windows "bin" is used").
#   INSTALL_LIB_DIR "lib"
#   )
# 
###############################################################################
FUNCTION(build_paraview_client BPC_NAME)
  PV_PARSE_ARGUMENTS(BPC 
    "APPLICATION_NAME;TITLE;ORGANIZATION;SPLASH_IMAGE;VERSION_MAJOR;VERSION_MINOR;VERSION_PATCH;BUNDLE_ICON;APPLICATION_ICON;REQUIRED_PLUGINS;OPTIONAL_PLUGINS;PVMAIN_WINDOW;PVMAIN_WINDOW_INCLUDE;EXTRA_DEPENDENCIES;GUI_CONFIGURATION_XMLS;COMPRESSED_HELP_FILE;SOURCES;INSTALL_BIN_DIR;INSTALL_LIB_DIR"
    "MAKE_INITIALIZER_LIBRARY"
    ${ARGN}
    )

  # Version numbers are required. Throw an error is not set correctly.
  IF (NOT DEFINED BPC_VERSION_MAJOR OR NOT DEFINED BPC_VERSION_MINOR OR NOT DEFINED BPC_VERSION_PATCH)
    MESSAGE(ERROR 
      "VERSION_MAJOR, VERSION_MINOR and VERSION_PATCH must be specified")
  ENDIF (NOT DEFINED BPC_VERSION_MAJOR OR NOT DEFINED BPC_VERSION_MINOR OR NOT DEFINED BPC_VERSION_PATCH)

  # If no title is provided, make one up using the name.
  pv_set_if_not_set(BPC_TITLE "${BPC_NAME}")
  pv_set_if_not_set(BPC_APPLICATION_NAME "${BPC_NAME}")
  pv_set_if_not_set(BPC_ORGANIZATION "Humanity")
  pv_set_if_not_set(BPC_INSTALL_BIN_DIR "bin")
  IF (WIN32)
    pv_set_if_not_set(BPC_INSTALL_LIB_DIR "bin")
  ELSE (WIN32)
    pv_set_if_not_set(BPC_INSTALL_LIB_DIR
      "lib/${BPC_NAME}-${BPC_VERSION_MAJOR}.${BPC_VERSION_MINOR}")
  ENDIF (WIN32)

  SET (branding_source_dir "${ParaView_CMAKE_DIR}")

  # If APPLICATION_ICON is specified, use that for the windows executable.
  IF (WIN32 AND BPC_APPLICATION_ICON)
    FILE (WRITE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc"
      "// Icon with lowest ID value placed first to ensure application icon\n"
      "// remains consistent on all systems.\n"
      "IDI_ICON1 ICON \"@BPC_APPLICATION_ICON@\"")
    SET(exe_icon "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc")
  ENDIF (WIN32 AND BPC_APPLICATION_ICON)

  # If BPC_BUNDLE_ICON is set, setup the macosx bundle.
  IF (APPLE)
    IF (BPC_BUNDLE_ICON)
      GET_FILENAME_COMPONENT(bundle_icon_file "${BPC_BUNDLE_ICON}" NAME)
      SET(apple_bundle_sources ${bundle_icon_file})
      SET_SOURCE_FILES_PROPERTIES(
        ${BPC_BUNDLE_ICON}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
      )
      IF(QT_MAC_USE_COCOA)
        GET_FILENAME_COMPONENT(qt_menu_nib
          "@QT_QTGUI_LIBRARY_RELEASE@/Resources/qt_menu.nib"
          REALPATH)

        set(qt_menu_nib_sources
          "${qt_menu_nib}/classes.nib"
          "${qt_menu_nib}/info.nib"
          "${qt_menu_nib}/keyedobjects.nib"
          )
        SET_SOURCE_FILES_PROPERTIES(
          ${qt_menu_nib_sources}
          PROPERTIES
          MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
        )
      ELSE(QT_MAC_USE_COCOA)
        set(qt_menu_nib_sources)
      ENDIF(QT_MAC_USE_COCOA)
    ENDIF (BPC_BUNDLE_ICON)
    SET(MAKE_BUNDLE MACOSX_BUNDLE)
  ENDIF (APPLE)

  IF(WIN32)
    LINK_DIRECTORIES(${QT_LIBRARY_DIR})
  ENDIF(WIN32)

  # If splash image is not specified, use the standard ParaView splash image.
  pv_set_if_not_set(BPC_SPLASH_IMAGE "${branding_source_dir}/branded_splash.png")
  CONFIGURE_FILE("${BPC_SPLASH_IMAGE}"
                  ${CMAKE_CURRENT_BINARY_DIR}/SplashImage.img COPYONLY)
  SET (BPC_SPLASH_IMAGE ${CMAKE_CURRENT_BINARY_DIR}/SplashImage.img)
  GET_FILENAME_COMPONENT(BPC_SPLASH_RESOURCE ${BPC_SPLASH_IMAGE} NAME)
  SET (BPC_SPLASH_RESOURCE ":/${BPC_NAME}/${BPC_SPLASH_RESOURCE}")

  pv_set_if_not_set(BPC_PVMAIN_WINDOW "QMainWindow")
  pv_set_if_not_set(BPC_PVMAIN_WINDOW_INCLUDE "QMainWindow")

  SET (BPC_HAS_GUI_CONFIGURATION_XMLS 0)
  IF (BPC_GUI_CONFIGURATION_XMLS)
    SET (BPC_HAS_GUI_CONFIGURATION_XMLS 1)
  ENDIF (BPC_GUI_CONFIGURATION_XMLS)

  # Generate a resource file out of the splash image.
  GENERATE_QT_RESOURCE_FROM_FILES(
    "${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_generated.qrc" 
    "/${BPC_NAME}" ${BPC_SPLASH_IMAGE}) 

  GENERATE_QT_RESOURCE_FROM_FILES(
    "${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_configuration.qrc"
    "/${BPC_NAME}/Configuration"
    "${BPC_GUI_CONFIGURATION_XMLS}")

  SET (ui_resources
    "${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_generated.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_configuration.qrc"
    )

  IF (BPC_COMPRESSED_HELP_FILE)
    # If a help collection file is specified, create a resource from it so that
    # when the ParaView help system can locate it at runtime and show the
    # appropriate help when the user asks for it. The 
    set (outfile "${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_help.qrc")
    GENERATE_QT_RESOURCE_FROM_FILES("${outfile}"
      "/${BPC_APPLICATION_NAME}/Documentation"
      "${BPC_COMPRESSED_HELP_FILE};")
    SET_SOURCE_FILES_PROPERTIES(${outfile}
      PROPERTIES OBJECT_DEPENDS "${BPC_COMPRESSED_HELP_FILE}")
    SET (ui_resources ${ui_resources} "${outfile}")
  ENDIF (BPC_COMPRESSED_HELP_FILE)
  
  QT4_ADD_RESOURCES(rcs_sources
    ${ui_resources}
    )

  SOURCE_GROUP("Resources" FILES
    ${ui_resources}
    ${exe_icon}
    )

  SOURCE_GROUP("Generated" FILES
    ${rcs_sources}
    )

  CONFIGURE_FILE(${branding_source_dir}/branded_paraview_main.cxx.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${BPC_NAME}_main.cxx @ONLY)
  CONFIGURE_FILE(${branding_source_dir}/branded_paraview_initializer.cxx.in
                 ${CMAKE_CURRENT_BINARY_DIR}/pq${BPC_NAME}Initializer.cxx @ONLY)
  CONFIGURE_FILE(${branding_source_dir}/branded_paraview_initializer.h.in
                 ${CMAKE_CURRENT_BINARY_DIR}/pq${BPC_NAME}Initializer.h @ONLY)

  IF (NOT Q_WS_MAC)
    SET(pv_exe_name ${BPC_NAME}${PV_EXE_SUFFIX})
  ELSE (NOT Q_WS_MAC)
    SET(pv_exe_name ${BPC_NAME})
  ENDIF (NOT Q_WS_MAC)

  INCLUDE_DIRECTORIES(
    ${PARAVIEW_GUI_INCLUDE_DIRS}
    )

  # If BPC_MAKE_INITIALIZER_LIBRARY is set, then we are creating a separate
  # library for the initializer, otherwise we don't create a separate library to
  # keep things simple.
  SET (EXE_SRCS pq${BPC_NAME}Initializer.cxx
               ${rcs_sources}
               ${BPC_SOURCES})

  IF (BPC_MAKE_INITIALIZER_LIBRARY)
    ADD_LIBRARY(pq${BPC_NAME}Initializer SHARED 
                ${EXE_SRCS} 
                )
    SET (EXE_SRCS)
    TARGET_LINK_LIBRARIES(pq${BPC_NAME}Initializer
      pqApplicationComponents
      ${QT_QTMAIN_LIBRARY}
      ${BPC_EXTRA_DEPENDENCIES}
      )

    IF (PV_INSTALL_LIB_DIR)
      INSTALL(TARGETS pq${BPC_NAME}Initializer
            DESTINATION ${PV_INSTALL_LIB_DIR}
            COMPONENT BrandedRuntime)
    ENDIF (PV_INSTALL_LIB_DIR)
  ENDIF (BPC_MAKE_INITIALIZER_LIBRARY)

  SET (PV_EXE_LIST ${BPC_NAME})

  # needed to set up shared forwarding correctly.
  add_executable_with_forwarding2(pv_exe_suffix
                 "${PARAVIEW_LIBRARY_DIRS}"
                 "../${PARAVIEW_INSTALL_LIB_DIR}"
                 "${BPC_INSTALL_LIB_DIR}"
                 ${BPC_NAME} WIN32 ${MAKE_BUNDLE}
                 ${BPC_NAME}_main.cxx
                 ${exe_icon}
                 ${apple_bundle_sources}
                 ${qt_menu_nib_sources}
                 ${EXE_SRCS}
                 )
  SET (pv_exe_name ${BPC_NAME}${pv_exe_suffix})
  TARGET_LINK_LIBRARIES(${pv_exe_name}
    pqApplicationComponents
    ${QT_QTMAIN_LIBRARY}
    ${BPC_EXTRA_DEPENDENCIES}
    )

  IF(NOT DEFINED MAKE_BUNDLE)
    INSTALL(TARGETS ${BPC_NAME}
      DESTINATION ${BPC_INSTALL_BIN_DIR}
      COMPONENT BrandedRuntime)
  ENDIF(NOT MAKE_BUNDLE)

  IF (pv_exe_suffix)
    # Shared forwarding enabled.
    INSTALL(TARGETS ${pv_exe_name}
          DESTINATION ${BPC_INSTALL_LIB_DIR}
          COMPONENT BrandedRuntime)
  ENDIF (pv_exe_suffix)

  IF (BPC_MAKE_INITIALIZER_LIBRARY)
    TARGET_LINK_LIBRARIES(${pv_exe_name}
      pq${BPC_NAME}Initializer)
  ENDIF (BPC_MAKE_INITIALIZER_LIBRARY)

  IF (APPLE)
    IF (BPC_BUNDLE_ICON)
      SET_TARGET_PROPERTIES(${pv_exe_name} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE ${bundle_icon_file})
    ENDIF (BPC_BUNDLE_ICON)
    SET_TARGET_PROPERTIES(${pv_exe_name} PROPERTIES 
      MACOSX_BUNDLE_BUNDLE_NAME "${BPC_APPLICATION_NAME}")
  ENDIF (APPLE)

  # HACK. When employing shared forwarding, I need to expose the real target so
  # that dependencies can be added correctly. This will go away once we remove
  # VTK_USE_RPATH option all together.
  set (paraview_client_real_exe_name ${pv_exe_name} PARENT_SCOPE)
ENDFUNCTION(build_paraview_client)
