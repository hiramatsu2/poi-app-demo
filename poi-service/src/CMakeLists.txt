###########################################################################
# Copyright 2018 IoT.bzh
#
# author: Loïc Collignon <loic.collignon@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS $ENV{OE_QMAKE_PATH_HOST_BINS})

INCLUDE(FindPkgConfig)
find_package(Qt5 COMPONENTS Core Network REQUIRED)
pkg_check_modules(QTAPPFW REQUIRED qtappfw)

PROJECT_TARGET_ADD(poi-service)

add_executable(${PROJECT_NAME}
	main.cpp
	poiservice.cpp
	poiservice.h
	webclient.cpp
	webclient.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	LABELS "EXECUTABLE"
	PREFIX ""
	COMPILE_FLAGS "${EXTRAS_CFLAGS}"
	LINK_FLAGS "${BINDINGS_LINK_FLAG}"
	LINK_LIBRARIES "${EXTRAS_LIBRARIES}"
	OUTPUT_NAME "${TARGET_NAME}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${QTAPPFW_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Core
	Qt5::Network
	${QTAPPFW_LIBRARIES}
)

