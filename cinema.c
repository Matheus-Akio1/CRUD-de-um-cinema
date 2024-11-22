#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Estruturas de dados
typedef struct {
    int id;
    char titulo[50];
    char horario[10];
    float valor_ingresso;
    int duracao;
    int classificacao;
} Filme;

typedef struct {
    int id;
    int quantidade;
} ItemCarrinho;

typedef struct {
    ItemCarrinho items[10];
    int total_items;
} Carrinho;

// Funcoes auxiliares
void pause() {
    printf("\nPressione Enter para continuar... ");
    getchar();
    getchar();
    system("cls"); // Use "cls" no Windows
}

// Funcoes principais
void adicionarFilme() {
    FILE *file = fopen("filmes.txt", "a");
    if (file == NULL) {
        printf("❌ Erro ao abrir o arquivo!\n");
        return;
    }
    
    Filme filme;
    printf("🎬 ID do Filme: ");
    scanf("%d", &filme.id);
    printf("🎬 Titulo: ");
    scanf(" %[^\n]s", filme.titulo);
    printf("🕒 Horario (HH:MM): ");
    scanf("%s", filme.horario);
    printf("💲 Valor do Ingresso: ");
    scanf("%f", &filme.valor_ingresso);
    printf("🕔 Duracao (min): ");
    scanf("%d", &filme.duracao);
    printf("🔞 Classificacao indicativa: ");
    scanf("%d", &filme.classificacao);
    
    fprintf(file, "%d;%s;%s;%.2f;%d;%d\n", filme.id, filme.titulo, filme.horario, filme.valor_ingresso, filme.duracao, filme.classificacao);
    fclose(file);

    printf("✅ Filme adicionado com sucesso!\n");
    pause();
}

void exibirFilmes() {
    FILE *file = fopen("filmes.txt", "r");
    if (file == NULL) {
        printf("📂 Nenhum filme encontrado.\n");
        return;
    }
    
    Filme filme;
    printf("📜 Lista de Filmes:\n\n");
    while (fscanf(file, "%d;%49[^;];%9[^;];%f;%d;%d\n", &filme.id, filme.titulo, filme.horario, &filme.valor_ingresso, &filme.duracao, &filme.classificacao) != EOF) {
        printf("🎥 ID: %d\n📖 Titulo: %s\n🕒 Horario: %s\n💲 Valor: R$%.2f\n⏱ Duracao: %d minutos\n🔞 Classificacao: %d anos\n", 
               filme.id, filme.titulo, filme.horario, filme.valor_ingresso, filme.duracao, filme.classificacao);
        printf("----------------------\n");
    }
    fclose(file);
    pause();
}

void removerFilme(int id) {
    FILE *file = fopen("filmes.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (file == NULL || temp == NULL) {
        printf("❌ Erro ao abrir o arquivo!\n");
        return;
    }
    
    Filme filme;
    int encontrado = 0;
    while (fscanf(file, "%d;%49[^;];%9[^;];%f;%d;%d\n", &filme.id, filme.titulo, filme.horario, &filme.valor_ingresso, &filme.duracao, &filme.classificacao) != EOF) {
        if (filme.id != id) {
            fprintf(temp, "%d;%s;%s;%.2f;%d;%d\n", filme.id, filme.titulo, filme.horario, filme.valor_ingresso, filme.duracao, filme.classificacao);
        } else {
            encontrado = 1;
        }
    }
    
    fclose(file);
    fclose(temp);
    remove("filmes.txt");
    rename("temp.txt", "filmes.txt");
    
    if (encontrado)
        printf("✅ Filme removido com sucesso!\n");
    else
        printf("❌ Filme nao encontrado.\n");
    pause();
}

void adicionarAoCarrinho(Carrinho *carrinho, int id, int quantidade) {
    if (carrinho->total_items < 10) {
        carrinho->items[carrinho->total_items].id = id;
        carrinho->items[carrinho->total_items].quantidade = quantidade;
        carrinho->total_items++;
        printf("✅ Filme adicionado ao carrinho!\n");
    } else {
        printf("⚠️ Carrinho cheio!\n");
    }
    pause();
}

void escolherFormaPagamento(float total, Carrinho *carrinho, int pedidoID) {
    int opcao, parcelas;
    printf("💳 Escolha a forma de pagamento:\n1. A Vista\n2. Cartao Parcelado\n3. Pix\n");
    scanf("%d", &opcao);
    
    FILE *vendas = fopen("vendas.txt", "a");
    if (vendas == NULL) {
        printf("❌ Erro ao abrir arquivo de vendas.\n");
        return;
    }

    fprintf(vendas, "Pedido #%d\n", pedidoID);
    switch (opcao) {
        case 1:
            printf("Pagamento a vista no valor de R$%.2f realizado com sucesso!\n", total);
            fprintf(vendas, "Pagamento a vista: R$%.2f\n", total);
            break;
        case 2:
            printf("Digite o numero de parcelas: ");
            scanf("%d", &parcelas);
            printf("Pagamento em %dx de R$%.2f realizado com sucesso!\n", parcelas, total / parcelas);
            fprintf(vendas, "Pagamento parcelado em %d vezes de R$%.2f\n", parcelas, total / parcelas);
            break;
        case 3:
            printf("Pagamento via Pix no valor de R$%.2f realizado com sucesso!\n", total);
            fprintf(vendas, "Pagamento via Pix: R$%.2f\n", total);
            break;
        default:
            printf("Opção invalida! Compra nao realizada.\n");
            fclose(vendas);
            return;
    }
    carrinho->total_items = 0;
    printf("✅ Compra finalizada e carrinho limpo.\n");
    fprintf(vendas, "----------------------\n\n");
    fclose(vendas);
    pause();
}

void finalizarCompra(Carrinho *carrinho) {
    FILE *file = fopen("filmes.txt", "r");
    FILE *vendas = fopen("vendas.txt", "a");
    Filme filme;
    float total = 0.0;

    if (file == NULL || vendas == NULL) {
        printf("❌ Erro ao abrir arquivo de filmes ou vendas!\n");
        return;
    }

    static int pedidoID = 1;
    fprintf(vendas, "Pedido #%d:\n", pedidoID);

    printf("🛒 Itens no carrinho:\n");
    for (int i = 0; i < carrinho->total_items; i++) {
        rewind(file);
        while (fscanf(file, "%d;%49[^;];%9[^;];%f;%d;%d\n", &filme.id, filme.titulo, filme.horario, &filme.valor_ingresso, &filme.duracao, &filme.classificacao) != EOF) {
            if (filme.id == carrinho->items[i].id) {
                float subtotal = filme.valor_ingresso * carrinho->items[i].quantidade;
                total += subtotal;
                printf("🎬 Filme: %s | Quantidade: %d | Subtotal: R$%.2f\n", filme.titulo, carrinho->items[i].quantidade, subtotal);
                fprintf(vendas, "Filme: %s | Quantidade: %d | Subtotal: R$%.2f\n", filme.titulo, carrinho->items[i].quantidade, subtotal);
                break;
            }
        }
    }
    fclose(file);
    fprintf(vendas, "Total do Pedido: R$%.2f\n", total);
    printf("💰 Total: R$%.2f\n", total);

    escolherFormaPagamento(total, carrinho, pedidoID);
    pedidoID++;
}

void gerarRelatorioFilmes() {
    FILE *file = fopen("filmes.txt", "r");
    FILE *relatorio = fopen("relatorio_filmes.txt", "w");
    if (file == NULL || relatorio == NULL) {
        printf("❌ Erro ao gerar relatorio de filmes.\n");
        return;
    }
    
    Filme filme;
    fprintf(relatorio, "Relatorio de Filmes:\n\n");
    while (fscanf(file, "%d;%49[^;];%9[^;];%f;%d;%d\n", &filme.id, filme.titulo, filme.horario, &filme.valor_ingresso, &filme.duracao, &filme.classificacao) != EOF) {
        fprintf(relatorio, "ID: %d\nTitulo: %s\nHorario: %s\nValor: R$%.2f\nDuracao: %d minutos\nClassificacao: %d anos\n\n", filme.id, filme.titulo, filme.horario, filme.valor_ingresso, filme.duracao, filme.classificacao);
    }
    fclose(file);
    fclose(relatorio);
    printf("📜 Relatorio de filmes gerado com sucesso!\n");
    pause();
}

void gerarRelatorioVendas() {
    FILE *vendas = fopen("vendas.txt", "r");
    FILE *relatorio = fopen("relatorio_vendas.txt", "w");
    if (vendas == NULL || relatorio == NULL) {
        printf("❌ Erro ao gerar relatorio de vendas.\n");
        return;
    }
    
    char linha[100];
    fprintf(relatorio, "Relatorio de Vendas:\n\n");
    while (fgets(linha, sizeof(linha), vendas)) {
        fprintf(relatorio, "%s", linha);
    }
    
    fclose(vendas);
    fclose(relatorio);
    printf("📜 Relatorio de vendas gerado com sucesso!\n");
    pause();
}

// MAIN
int main() {

    SetConsoleOutputCP(CP_UTF8);

    int opcao, id, quantidade;
    Carrinho carrinho = { .total_items = 0 };
    
    do {
        printf("\n🎥 ------ Cinema ------ 🎥\n\n");
        printf("1. 📥 Adicionar Filme\n2. 📃 Exibir Filmes\n3. 🗑️ Remover Filme\n4. 🛒 Adicionar ao Carrinho\n5. 💸 Finalizar Compra\n6. 📜 Relatorio de Filmes\n7. 📜 Relatorio de Vendas\n0. 🔚 Sair\n");
        printf("\nEscolha: ");
        scanf("%d", &opcao);
        printf("----------------------\n\n");
        
        switch (opcao) {
            case 1:
                adicionarFilme();
                break;
            case 2:
                exibirFilmes();
                break;
            case 3:
                printf("🗑️ ID do Filme para remover: ");
                scanf("%d", &id);
                removerFilme(id);
                break;
            case 4:
                printf("🛒 ID do Filme para adicionar ao carrinho: ");
                scanf("%d", &id);
                printf("🔢 Quantidade: ");
                scanf("%d", &quantidade);
                adicionarAoCarrinho(&carrinho, id, quantidade);
                break;
            case 5:
                finalizarCompra(&carrinho);
                break;
            case 6:
                gerarRelatorioFilmes();
                break;
            case 7:
                gerarRelatorioVendas();
                break;
            case 0:
                printf("🔚 Saindo...\n");
                break;
            default:
                printf("⚠️ Opção inválida!\n");
                pause();
        }
    } while (opcao != 0);

    return 0;
}
