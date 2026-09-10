#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "votacao.grpc.pb.h"

class VotacaoServiceImpl final : public votacao::VotacaoService::Service {
private:
    struct Candidato {
        int id;
        std::string nome;
        int votos;
    };

    std::vector<Candidato> candidatos = {
        {1, "Alex", 0},
        {2, "Maria", 0},
        {3, "Pedro", 0}
    };

    // Guarda os eleitores que já votaram.
    std::unordered_set<int> eleitores_que_votaram;

    // Protege os dados compartilhados contra acessos simultâneos.
    std::mutex mutex_votacao;

public:
    grpc::Status Votar(
        grpc::ServerContext* context,
        const votacao::VotarRequest* request,
        votacao::VotarResponse* response
    ) override {

        // Protege esta operação contra duas requisições simultâneas.
        std::lock_guard<std::mutex> lock(mutex_votacao);

        int eleitor_id = request->eleitor_id();
        int candidato_id = request->candidato_id();

        // Verifica se a votação já foi realizada por este eleitor.
        if (eleitores_que_votaram.count(eleitor_id) > 0) {
            response->set_sucesso(false);
            response->set_mensagem(
                "Eleitor " + std::to_string(eleitor_id) +
                " ja realizou seu voto."
            );

            return grpc::Status::OK;
        }

        // Procura o candidato.
        for (auto& candidato : candidatos) {
            if (candidato.id == candidato_id) {

                candidato.votos++;

                eleitores_que_votaram.insert(eleitor_id);

                response->set_sucesso(true);
                response->set_mensagem(
                    "Voto registrado para " + candidato.nome + "."
                );

                std::cout
                    << "Eleitor " << eleitor_id
                    << " votou em " << candidato.nome
                    << ". Total de votos: "
                    << candidato.votos
                    << '\n';

                return grpc::Status::OK;
            }
        }

        // Candidato não encontrado.
        response->set_sucesso(false);
        response->set_mensagem(
            "Candidato " + std::to_string(candidato_id) +
            " nao encontrado."
        );

        return grpc::Status::OK;
    }
    grpc::Status ConsultarResultado(
        grpc::ServerContext* context,
        const votacao::ConsultarResultadoRequest* request,
        votacao::ConsultarResultadoResponse* response
    ) override {

        std::lock_guard<std::mutex> lock(mutex_votacao);

        int total_votos = 0;

        for (const auto& candidato : candidatos) {
            auto* candidato_response = response->add_candidatos();

            candidato_response->set_id(candidato.id);
            candidato_response->set_nome(candidato.nome);
            candidato_response->set_votos(candidato.votos);

            total_votos += candidato.votos;
        }

        response->set_total_votos(total_votos);

        return grpc::Status::OK;
    }


    grpc::Status ListarCandidatos(
        grpc::ServerContext* context,
        const votacao::ListarCandidatosRequest* request,
        votacao::ListarCandidatosResponse* response
    ) override {

        std::lock_guard<std::mutex> lock(mutex_votacao);

        for (const auto& candidato : candidatos) {
            auto* candidato_response = response->add_candidatos();

            candidato_response->set_id(candidato.id);
            candidato_response->set_nome(candidato.nome);
            candidato_response->set_votos(candidato.votos);
        }

        return grpc::Status::OK;
    }

    
    grpc::Status VerificarEleitor(
        grpc::ServerContext* context,
        const votacao::VerificarEleitorRequest* request,
        votacao::VerificarEleitorResponse* response
    ) override {

        std::lock_guard<std::mutex> lock(mutex_votacao);

        int eleitor_id = request->eleitor_id();

        bool ja_votou =
            eleitores_que_votaram.count(eleitor_id) > 0;

        response->set_ja_votou(ja_votou);

        return grpc::Status::OK;
    }
};

int main() {
    const std::string server_address = "0.0.0.0:50051";

    VotacaoServiceImpl service;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(
        server_address,
        grpc::InsecureServerCredentials()
    );

    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(
        builder.BuildAndStart()
    );

    if (!server) {
        std::cerr << "Erro ao iniciar o servidor.\n";
        return 1;
    }

    std::cout
        << "Servidor gRPC iniciado em "
        << server_address
        << '\n';

    server->Wait();

    return 0;
}