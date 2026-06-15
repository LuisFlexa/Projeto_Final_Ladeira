#include "util_classe.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>

/* ----------------------------------------------------------------------- */
/* Versao                                                                  */
/* ----------------------------------------------------------------------- */
double UtilClasse::versao_legivel(ArquivoClasse* arq)
{
    u2 mv = arq->major_version;
    if (mv < 45 || mv > 52) {
        return 0;
    }
    if (mv <= 48) {
        /* Java 1.1 .. 1.4 */
        return 1.0 + (mv - 44) * 0.1;
    }
    /* Java 5 (49) .. Java 8 (52) */
    return 5 + (mv - 49);
}

double UtilClasse::versao_para_msg_erro(ArquivoClasse* arq)
{
    u2 mv = arq->major_version;
    if (mv < 45 || mv > 52) {
        return 5 + (mv - 49);
    }
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Comparacao de strings UTF-8                                             */
/* ----------------------------------------------------------------------- */
bool UtilClasse::utf8_igual_a(ConstUtf8Info entry, const char* alvo)
{
    size_t n = std::strlen(alvo);
    if (entry.length != n) {
        return false;
    }
    return std::memcmp(entry.bytes, alvo, n) == 0;
}

/* ----------------------------------------------------------------------- */
/* Indentacao                                                              */
/* ----------------------------------------------------------------------- */
void UtilClasse::indentar_arquivo(FILE* out, uint8_t n)
{
    while (n--) {
        std::fputc('\t', out);
    }
}

void UtilClasse::indentar_tela(uint8_t n)
{
    while (n--) {
        std::fputc('\t', stdout);
    }
}

/* ----------------------------------------------------------------------- */
/* stringstream -> C-string alocada                                        */
/* ----------------------------------------------------------------------- */
const char* UtilClasse::ss_para_cstr(const std::stringstream& ss)
{
    std::string tmp = ss.str();
    char* res = static_cast<char*>(std::malloc(tmp.size() + 1));
    if (res == nullptr) {
        std::cerr << "Falha de alocacao em ss_para_cstr\n";
        std::exit(EXIT_FAILURE);
    }
    std::memcpy(res, tmp.data(), tmp.size());
    res[tmp.size()] = '\0';
    return res;
}

/* ----------------------------------------------------------------------- */
/* Conferencia do nome da classe vs nome do arquivo                        */
/* ----------------------------------------------------------------------- */
static std::string basename_sem_ext(std::string nome)
{
    /* remove ate' o ultimo separador (/ ou \), ignorando case */
    size_t pos = nome.find_last_of("/\\");
    if (pos != std::string::npos) {
        nome = nome.substr(pos + 1);
    }
    /* remove sufixo .class/.CLASS (comparacao case-insensitive) */
    const size_t k = 6;
    if (nome.size() >= k) {
        std::string tail = nome.substr(nome.size() - k);
        for (char& c : tail) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (tail == ".class") {
            nome.resize(nome.size() - k);
        }
    }
    return nome;
}

bool UtilClasse::conferir_nome_classe(std::string caminho_arq, std::string nome_classe)
{
    /* o nome no pool pode vir como "subdir/Foo" - so' interessa o basename */
    return basename_sem_ext(caminho_arq) == basename_sem_ext(nome_classe);
}
