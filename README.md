# Sistema de Votação Distribuída

Sistema de votação distribuída desenvolvido em **C++**, utilizando **gRPC** e **Protocol Buffers** para comunicação entre clientes e servidor.

O projeto foi desenvolvido para a disciplina de **Sistemas Distribuídos**, com o objetivo de demonstrar o funcionamento de **Remote Procedure Call (RPC)**, comunicação cliente-servidor, múltiplos clientes e processamento concorrente de requisições.

## Tecnologias

* **C++17**
* **gRPC**
* **Protocol Buffers (Protobuf)**
* **CMake**
* **Ninja**
* **MSYS2 UCRT64**

## Arquitetura

A aplicação utiliza uma arquitetura cliente-servidor:

```text
                     REDE
                      │
          ┌───────────┴───────────┐
          │                       │
     Cliente 1               Cliente 2
          │                       │
          └───────────┬───────────┘
                      │
                     gRPC
                      │
                      ▼
              ┌───────────────┐
              │    Servidor   │
              │               │
              │  Candidatos   │
              │  Eleitores    │
              │  Votos        │
              └───────────────┘
```

O servidor mantém o estado da votação, enquanto os clientes realizam chamadas RPC para consultar informações e registrar votos.

## Funcionalidades

O sistema disponibiliza as seguintes operações:

* Listar candidatos
* Verificar se o eleitor já votou
* Registrar voto
* Consultar resultado da votação
* Encerrar o cliente
* Suporte a múltiplos clientes simultâneos
* Comunicação entre computadores através da rede local

Cada cliente recebe automaticamente um identificador de eleitor durante sua execução.

## RPC

A interface do serviço é definida no arquivo:

```text
src/proto/votacao.proto
```

Exemplo:

```proto
service VotacaoService {
    rpc Votar(VotarRequest)
        returns (VotarResponse);

    rpc ConsultarResultado(ConsultarResultadoRequest)
        returns (ConsultarResultadoResponse);
}
```

O arquivo `.proto` funciona como o contrato de comunicação entre cliente e servidor. A partir dele, o **Protocol Buffer Compiler (`protoc`)** gera automaticamente os arquivos C++ utilizados pelo gRPC.

## Estrutura do projeto

```text
src/
├── client/
│   └── client.cpp
│
├── server/
│   └── server.cpp
│
├── proto/
│   └── votacao.proto
│
├── generated/
│   ├── votacao.pb.cc
│   ├── votacao.pb.h
│   ├── votacao.grpc.pb.cc
│   └── votacao.grpc.pb.h
│
├── build/
│   ├── voting_client.exe
│   └── voting_server.exe
│
└── CMakeLists.txt
```

## Compilação

Com o ambiente **MSYS2 UCRT64** configurado, dentro da pasta `build`:

```bash
cmake --build .
```

Caso a pasta `build` esteja vazia ou o projeto esteja sendo configurado pela primeira vez:

```bash
cmake .. -G Ninja
cmake --build .
```

## Execução

### Servidor

```bash
./voting_server.exe
```

O servidor será iniciado na porta:

```text
0.0.0.0:50051
```

`0.0.0.0` permite que o servidor aceite conexões através das interfaces de rede disponíveis na máquina.

### Cliente

Em outro terminal:

```bash
./voting_client.exe
```

O cliente solicitará o endereço IP do servidor:

```text
Digite o IP do servidor: 192.168.1.11
```

A porta utilizada pelo serviço permanece:

```text
50051
```

Portanto, a conexão será estabelecida através de:

```text
192.168.1.11:50051
```

## Execução em computadores diferentes

O servidor e os clientes podem ser executados em computadores diferentes conectados à mesma rede local.

Exemplo:

```text
Computador A
Servidor
IP: 192.168.1.11
Porta: 50051

        │
        │ Rede local
        │
        ├───────────────┐
        │               │
        ▼               ▼

Computador B       Computador C
Cliente 1          Cliente 2
```

Nos clientes, basta informar o endereço IP do computador que está executando o servidor.

## Concorrência

O servidor pode receber requisições de múltiplos clientes simultaneamente.

Como os votos e os eleitores são armazenados em estruturas compartilhadas, o acesso a esses dados é protegido através de `std::mutex`.

Exemplo:

```cpp
std::lock_guard<std::mutex> lock(mutex_votacao);
```

Isso evita condições de corrida durante operações como:

* verificar se um eleitor já votou;
* registrar um voto;
* atualizar a quantidade de votos.

## Demonstração

Uma demonstração típica pode utilizar:

```text
Computador 1 → Servidor gRPC
Computador 2 → Cliente 1
Computador 3 → Cliente 2
```

Os dois clientes podem realizar operações simultaneamente sobre a mesma votação mantida pelo servidor.

## Objetivo acadêmico

O projeto demonstra, na prática, conceitos de Sistemas Distribuídos, incluindo:

* Remote Procedure Call (RPC);
* arquitetura cliente-servidor;
* comunicação através de rede;
* serialização de dados com Protocol Buffers;
* comunicação utilizando gRPC;
* múltiplos clientes;
* concorrência;
* estado compartilhado no servidor;
* tratamento de chamadas remotas.

## Status

Projeto desenvolvido para fins acadêmicos na disciplina de **Sistemas Distribuídos**.
