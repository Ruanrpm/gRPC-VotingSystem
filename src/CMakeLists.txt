cmake_minimum_required(VERSION 3.20)

project(VotingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Localização das bibliotecas instaladas pelo MSYS2 UCRT64
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

# Bibliotecas necessárias
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Arquivos gerados pelo protoc
set(PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/votacao.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/votacao.grpc.pb.cc
)

set(PROTO_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/votacao.pb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/votacao.grpc.pb.h
)

# Biblioteca com o código gerado pelo Protobuf/gRPC
add_library(voting_proto
    ${PROTO_FILES}
    ${PROTO_HEADERS}
)

target_include_directories(voting_proto
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_link_libraries(voting_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
)

# =========================
# SERVIDOR
# =========================

add_executable(voting_server
    server/server.cpp
)

target_link_libraries(voting_server
    PRIVATE
        voting_proto
)

# =========================
# CLIENTE
# =========================

add_executable(voting_client
    client/client.cpp
)

target_link_libraries(voting_client
    PRIVATE
        voting_proto
)