#include <iostream>
#include <memory>
#include <random>
#include <string>

#include <grpcpp/grpcpp.h>

#include "votacao.grpc.pb.h"

// Gera um ID aleatório entre 1000 e 9999.
int gerarIdEleitor() {
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_int_distribution<int> distribution(1000, 9999);

    return distribution(generator);
}

// Lista os candidatos disponíveis.
void listarCandidatos(
    std::unique_ptr<votacao::VotacaoService::Stub>& stub
) {
    votacao::ListarCandidatosRequest request;
    votacao::ListarCandidatosResponse response;

    grpc::ClientContext context;

    grpc::Status status = stub->ListarCandidatos(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        std::cerr
            << "Erro ao listar candidatos: "
            << status.error_message()
            << "\n";

        return;
    }

    std::cout << "\n========== CANDIDATOS ==========\n";

    for (const auto& candidato : response.candidatos()) {
        std::cout
            << candidato.id()
            << " - "
            << candidato.nome()
            << "\n";
    }

    std::cout << "================================\n";
}

// Realiza o voto.
void votar(
    std::unique_ptr<votacao::VotacaoService::Stub>& stub,
    int eleitor_id
) {
    // Primeiro verificamos se esse eleitor já votou.
    votacao::VerificarEleitorRequest verificar_request;
    votacao::VerificarEleitorResponse verificar_response;

    verificar_request.set_eleitor_id(eleitor_id);

    grpc::ClientContext verificar_context;

    grpc::Status verificar_status =
        stub->VerificarEleitor(
            &verificar_context,
            verificar_request,
            &verificar_response
        );

    if (!verificar_status.ok()) {
        std::cerr
            << "Erro ao verificar eleitor: "
            << verificar_status.error_message()
            << "\n";

        return;
    }

    if (verificar_response.ja_votou()) {
        std::cout
            << "\nEste eleitor ja realizou seu voto.\n";

        return;
    }

    // Mostra os candidatos antes de pedir o voto.
    listarCandidatos(stub);

    int candidato_id;

    std::cout << "\nDigite o numero do candidato: ";
    std::cin >> candidato_id;

    votacao::VotarRequest request;
    votacao::VotarResponse response;

    request.set_eleitor_id(eleitor_id);
    request.set_candidato_id(candidato_id);

    grpc::ClientContext context;

    grpc::Status status = stub->Votar(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        std::cerr
            << "Erro na chamada RPC Votar: "
            << status.error_message()
            << "\n";

        return;
    }

    std::cout
        << "\nServidor: "
        << response.mensagem()
        << "\n";
}

// Consulta o resultado atual da votação.
void consultarResultado(
    std::unique_ptr<votacao::VotacaoService::Stub>& stub
) {
    votacao::ConsultarResultadoRequest request;
    votacao::ConsultarResultadoResponse response;

    grpc::ClientContext context;

    grpc::Status status = stub->ConsultarResultado(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        std::cerr
            << "Erro ao consultar resultado: "
            << status.error_message()
            << "\n";

        return;
    }

    std::cout << "\n========== RESULTADO ==========\n";

    for (const auto& candidato : response.candidatos()) {
        std::cout
            << candidato.id()
            << " - "
            << candidato.nome()
            << ": "
            << candidato.votos()
            << " votos\n";
    }

    std::cout
        << "Total de votos: "
        << response.total_votos()
        << "\n";

    std::cout << "================================\n";
}

// Verifica se o eleitor atual já votou.
void verificarMeuVoto(
    std::unique_ptr<votacao::VotacaoService::Stub>& stub,
    int eleitor_id
) {
    votacao::VerificarEleitorRequest request;
    votacao::VerificarEleitorResponse response;

    request.set_eleitor_id(eleitor_id);

    grpc::ClientContext context;

    grpc::Status status = stub->VerificarEleitor(
        &context,
        request,
        &response
    );

    if (!status.ok()) {
        std::cerr
            << "Erro ao verificar eleitor: "
            << status.error_message()
            << "\n";

        return;
    }

    std::cout << "\n";

    if (response.ja_votou()) {
        std::cout << "Este eleitor ja realizou seu voto.\n";
    } else {
        std::cout << "Este eleitor ainda nao votou.\n";
    }
}

// Exibe o menu principal.
void exibirMenu() {
    std::cout << "\n";
    std::cout << "========== MENU ==========\n";
    std::cout << "1 - Listar candidatos\n";
    std::cout << "2 - Votar\n";
    std::cout << "3 - Consultar resultado\n";
    std::cout << "4 - Verificar meu voto\n";
    std::cout << "0 - Sair\n";
    std::cout << "==========================\n";
}

int main() {

    std::cout << "====================================\n";
    std::cout << "   SISTEMA DE VOTACAO DISTRIBUIDA   \n";
    std::cout << "====================================\n\n";

    // =========================================================
    // IP DO SERVIDOR
    // =========================================================

    std::string server_ip;

    std::cout << "Digite o IP do servidor: ";
    std::cin >> server_ip;

    const std::string server_address =
        server_ip + ":50051";

    std::cout
        << "\nConectando ao servidor em "
        << server_address
        << "...\n";

    // =========================================================
    // ID DO ELEITOR
    // =========================================================

    const int eleitor_id = gerarIdEleitor();

    std::cout
        << "Seu ID de eleitor: "
        << eleitor_id
        << "\n";

    // =========================================================
    // CONEXAO COM O SERVIDOR
    // =========================================================

    auto channel = grpc::CreateChannel(
        server_address,
        grpc::InsecureChannelCredentials()
    );

    auto stub = votacao::VotacaoService::NewStub(channel);

    std::cout
        << "Conexao configurada com sucesso.\n";

    // =========================================================
    // MENU PRINCIPAL
    // =========================================================

    int opcao;

    do {

        exibirMenu();

        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        switch (opcao) {

            case 1:
                listarCandidatos(stub);
                break;

            case 2:
                votar(stub, eleitor_id);
                break;

            case 3:
                consultarResultado(stub);
                break;

            case 4:
                verificarMeuVoto(stub, eleitor_id);
                break;

            case 0:
                std::cout
                    << "\nEncerrando cliente...\n";
                break;

            default:
                std::cout
                    << "\nOpcao invalida.\n";
                break;
        }

    } while (opcao != 0);

    return 0;
}