#include "Bibliotecas/C++/AnalyticTableaux.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Expression{

    public:
        string exp, exp1, exp2;
        int val, val1, val2;
        int isAlpha;
        char op;
        
        Expression(){
            this->exp = "";
            this->exp1 = "";
            this->exp2 = "";
            this->val = -1;
            this->val1 = -1;
            this->val2 = -1;
            this->isAlpha = 0;
            this->op = ' ';
        }


        Expression(string expOrig, int val, string exp_1, string exp_2, int val_1, int val_2, int flagAlpha, char oper){
            this->exp = expOrig;
            this->exp1 = exp_1;
            this->exp2 = exp_2;
            this->val = val;
            this->val1 = val_1;
            this->val2 = val_2;
            this->isAlpha = flagAlpha;
            this->op = oper;
        }
};

// Dado um nó, a função separa as expressões e o operador principal, adicionando já o valor
//verdade de cada uma, dependendo do operador principal e do valor verdade da expressão principal
Expression getOperator(Node* node){
    int i = 0, qntE = 0, qntD = 0, val_1 = -1, val_2 = -1, flagAlpha = 0;
    char oper;
    string exp_1, exp_2;
    int val = node->getTruthValue();
    string expRec = node->getExpression();
    
    // Verifica se a operação é negação para poder começar a ler do índice certo a subexpressão
    if(expRec[1] == '~'){
        oper = '~';
        i = 2;
    }

    // Leitura da subexpressão à esquerda do operador principal (ou da subexpressão única da negação)
    while(true){
        if(expRec[i] == ')')
            qntD++;
        else if(expRec[i] == '(')
            qntE++;
    
        if(i != 0 || expRec[0] != '(')
            exp_1.push_back(expRec[i]);  

        if((oper == '~') && (qntE == qntD)){
            i = i + 2;
            break;
        }
        else if((oper != '~') && (((qntE == qntD + 1) && (qntD != 0))  || (qntD == qntE))){
            i = i + 2; 
            oper = expRec[i];           
            break;
        }
        else if((i == 1) && (expRec[i] != '(')){
            i = i + 2;
            oper = expRec[i];
            break;
        }
        else if((i == 0) && (expRec[i] != '(')){
            oper = ' '; 
            break;
        }
        i++;      
    }   

    // "Decodificação" do operador e valores verdades das subexpressões, verificando o tipo da regra também (se é alpha ou não) 
    if(oper == '~'){
        val_2 = -1;
        flagAlpha = 1;
        if(val == 0){
            val_1 = 1;
        }
        else if(val == 1){
            val_1 = 0;
        }
    } 
    else{
        if(oper == '&'){
            if(val == 0){
                flagAlpha = 0;
                val_1 = 0;
                val_2 = 0;
            }
            else if(val == 1){
                flagAlpha = 1;
                val_1 = 1;
                val_2 = 1;
            }
        }   
        else if(oper == 'v'){
            if(val == 0){
                flagAlpha = 1;
                val_1 = 0;
                val_2 = 0;
            }
            else if(val == 1){
                flagAlpha = 0;
                val_1 = 1;
                val_2 = 1;
            }
        }
        else if(oper == '>'){
            if(val == 0){
                flagAlpha = 1;
                val_1 = 1;
                val_2 = 0;
            }
            else if(val == 1){
                flagAlpha = 0;
                val_1 = 0;
                val_2 = 1;
            }
        }

        // Leitura da subexpressão à direita do operador principal
        i = i + 2;
        qntD = 0;
        qntE = 0;
        while(true){
            if(expRec[i] == ')')
                qntD++;
            else if(expRec[i] == '(')
                qntE++;
            
    
            exp_2.push_back(expRec[i]);
            i++;

            if((qntD == qntE + 1)  || (qntD == qntE))
                break;
        }
    }
    //  Cria e retorna um objeto do tipo "Expression" contendo as informações necessárias para a aplicação da regra
    Expression EXP = Expression(expRec,val,exp_1,exp_2,val_1,val_2,flagAlpha,oper);
    return EXP;

}

// Ordenar as regras que vão ser aplicadas (mergesort) -> regras alpha primeiro 
vector<Expression> merge(vector<Expression> vec, vector<Expression> auxVec, int l, int h, int r){
    for(int p = l; p < r; p++){
        auxVec[p] = vec[p];
    }
    int i = l, j = h;
    for(int k = l; k < r; k++){
        if(j == r){
            vec[k] = auxVec[i];
            i++;
        }
        else if(i == h){
            vec[k] = auxVec[j];
            j++;
        }
        else{
            if(auxVec[i].isAlpha >= auxVec[j].isAlpha){
                vec[k] = auxVec[i];
                i++;
            }
            else{
                vec[k] = auxVec[j];
                j++;
            }
        }
    }
    return vec;
}
vector<Expression> msort(vector<Expression> vec, vector<Expression> auxVec, int l, int r){
    if((r - l) == 1)
        return vec;
    else{
        int h = (l + r) / 2;
        vec = msort(vec, auxVec,l,h);
        vec = msort(vec,auxVec,h,r);
        vec = merge(vec,auxVec,l,h,r);
    }
    return vec;
}
vector<Expression> sortVec(vector<Expression> vec){
    vector<Expression> auxVec = vec;
    int n = vec.size();

    vec = msort(vec,auxVec,0,n);
    return vec;
}

// Aplicar regras
vector<Node*> applyRule(Expression expression, Node* node) {
    vector<Node*> insertedNodes;

    // Se for negação, a regra é do tipo alpha e só insere 1 nó na frente com a subexpressão 1 e valor-verdade 1
    if(expression.op == '~'){
        insertedNodes = node->insertFront(expression.exp1, expression.val1);
    } 
    /* Se não, se for um "E", verifica o valor verdade da proposição principal e insere 2 nós na frente se for 1 
    (com as subexpressões e valores-verdades previamente "decodificados"), ou 2 nós lado a lado se for 0*/
    else if(expression.op == '&'){
        if(expression.val == 1) {
            insertedNodes = node->insertFront(expression.exp1, expression.val1, expression.exp2, expression.val2);
        }
        else{
            insertedNodes = node->insertSides(expression.exp1, expression.val1, expression.exp2, expression.val2);
        }
    }
    /* Se não, se for um "OU" ou "SE-ENTÃO", verifica o valor verdade da proposição principal e insere 2 nós na frente 
    se for 0 (com as subexpressões e valores-verdades previamente "decodificados"), ou 2 nós lado a lado se for 1*/
    else if(expression.op == 'v' || expression.op == '>'){
        if(expression.val == 0) {
            insertedNodes = node->insertFront(expression.exp1, expression.val1, expression.exp2, expression.val2);
        }
        else{
            insertedNodes = node->insertSides(expression.exp1, expression.val1, expression.exp2, expression.val2);
        }
    }

    // Marca a regra como aplicada e retorna o vetor com os nós inseridos
    node->markApplied();
    return insertedNodes;
}

// Verifica a presença de contradição no tableaux e indica que o caminho está fechado caso exista alguma nele
void checkContradictions(vector<Node*> insertedNodes) {
    for (int i = 0; i < insertedNodes.size(); i++) {
        if(insertedNodes[i]->checkContradiction())
            insertedNodes[i]->markContradiction();
    }
}


int main() {
    int n;
    char c;
    string line;
    // Abertura do arquivo de escrita -> "Saida.out"
    ofstream outfile ("Entradas _ Saídas/Teste Geral #1/Saida.out");
    if (outfile.is_open()){
        // Leitura do arquivo de entrada
        ifstream infile;
        // Abertura do arquivo de leitura -> "Entrada.in"
        infile.open("Entradas _ Saídas/Teste Geral #1/Entrada.in");
        if(infile.is_open()){
            // Lê a quantidade de problemas contidados no arquivo (guarda em n) e o \n (guarda em c)
            infile >> n >> c;
            for(int j = 0; j < n; j++){
                int qntE = 0, qntD = 0, i = 0, flagFim = 0, flagContido = 0;
                vector<string> expressoes;
                string pergunta, expressao;

                getline (infile,line);

                int tam = line.size();                
                if(tam > 0){
                    // Leitura da proposição inicial
                    while(true){
                        expressao.push_back(line[i]);                        
                        i++;
                        if(line[i] == 'e'){
                            expressao.pop_back();
                            break;
                        }
                    }                    
                    expressoes.push_back(expressao);
                    expressao.clear();

                    // Leitura da pergunta
                    while(true){
                        pergunta.push_back(line[i]);
                        i++;
                        if(line[i] == '?'){
                            flagFim = 1;
                            break;
                        }
                        else if(line[i] == '{'){
                            flagFim = 0;
                            pergunta.pop_back();
                            pergunta.pop_back();
                            pergunta.pop_back();
                            pergunta.pop_back();
                            i++;
                            break;
                        }
                    }

                    // Caso exista, leitura do conjunto de proposições
                    while(flagFim == 0){
                        expressao.push_back(line[i]); 
                        i++;
                        if(line[i] == ','){
                            expressoes.push_back(expressao);
                            expressao.clear();
                            i = i + 2;
                        }
                        else if(line[i] == '}'){
                            expressoes.push_back(expressao);
                            flagFim = 1;
                        }
                    }

                    // Atribuição da valoração-verdade da proposição inicial de acordo com a pergunta
                    int value;
                    if(pergunta == "e satisfativel" || pergunta == "e insatisfativel")
                        value = 1;
                    else if(pergunta == "e refutavel" || pergunta == "e tautologia" || pergunta == "e consequencia logica")
                        value = 0;

                    // Insere a proposição inicial e seu valor-verdade no tableaux
                    Node tableaux = Node(expressoes[0],value);

                    // Atribuição da valoração-verdade das proposições do conjunto de proposições, caso a pergunta seja "e consequencia logica"
                    if(pergunta == "e consequencia logica"){
                        for(int m = 1; m < expressoes.size(); m++){
                            if(expressoes[m] == expressoes[0])
                                flagContido = 1;
                            tableaux.insertFront(expressoes[m],1);
                        }   
                    }
                    if(flagContido == 0){
                        // Verifica todos os nós do tableaux cuja regra ainda não tinha sido aplicada
                        vector<Node*> appliableNodes = tableaux.getAppliableNodes();
                        
                        vector<Expression> ordAppliableNodes;
                        // As regras a serem aplicadas são colocadas em um novo vetor já "decodificadas"
                        for(Node* node : appliableNodes){
                            ordAppliableNodes.push_back(getOperator(node));
                        }
                        // Ordena as regras à serem aplicadas -> alphas primeiro
                        if(!ordAppliableNodes.empty())
                            ordAppliableNodes = sortVec(ordAppliableNodes);
                            
                        // Enquanto o tableaux não fechar e ainda existirem regras não aplicadas, aplica a regra de maior prioridade e verifica 
                        //se há contradição em algum caminho do tableaux
                        while(!tableaux.isClosed() && !appliableNodes.empty()) {
                                for(Node* no : appliableNodes){
                                    if(no->getExpression() == ordAppliableNodes[0].exp && no->getTruthValue() == ordAppliableNodes[0].val){
                                        vector<Node*> insertedNodes = applyRule(ordAppliableNodes[0], no);
                                        checkContradictions(insertedNodes); 
                                        
                                    }                            
                                }
                            // Atualiza os vetores com o restante dos nós com regras ainda não aplicadas
                            appliableNodes = tableaux.getAppliableNodes();
                            ordAppliableNodes.clear();
                            for(Node* node : appliableNodes){
                                ordAppliableNodes.push_back(getOperator(node));
                            }
                            if(!ordAppliableNodes.empty())
                                ordAppliableNodes = sortVec(ordAppliableNodes);       
                        }                    

                        // Escrita da resposta
                        if(!tableaux.isClosed()){
                            if(pergunta == "e satisfativel")
                                outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                            else if(pergunta == "e insatisfativel")
                                outfile << "Problema #" << j+1 << endl << "Nao, nao " << pergunta << "." << endl;
                            else if(pergunta == "e refutavel")
                                outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                            else if(pergunta == "e tautologia")
                                outfile << "Problema #" << j+1 << endl << "Nao, nao " << pergunta << "." << endl;
                            else if(pergunta == "e consequencia logica")
                                outfile << "Problema #" << j+1 << endl << "Nao, nao " << pergunta << "." << endl;
                        }
                        else{
                            if(pergunta == "e satisfativel")
                                outfile << "Problema #" << j+1 << endl << "Nao, nao " << pergunta << "." << endl;
                            else if(pergunta == "e insatisfativel")
                                outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                            else if(pergunta == "e refutavel")
                                outfile << "Problema #" << j+1 << endl << "Nao, nao " << pergunta << "." << endl;
                            else if(pergunta == "e tautologia")
                                outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                            else if(pergunta == "e consequencia logica")
                                outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                        } 
                        if(j != n-1)
                            outfile << endl; 
                    } 

                    /* Caso o conjunto de proposições já contenha a proposição principal e a pergunta for "e consequencia logica", 
                    então a resposta vai ser verdadeira, pois sempre que todas as proposições do conjunto forem verdadeiras, 
                    então a principal também será, pois ela faz parte do conjunto; e quando não houver como todas as proposições do conjunto 
                    serem verdadeiras ao mesmo tempo, então a proposição principal vai ser consequencia por vacuidade.*/
                    else{
                        outfile << "Problema #" << j+1 << endl << "Sim, " << pergunta << "." << endl;
                        if(j != n-1)
                            outfile << endl; 
                    }                
                }
            }
            infile.close();
        }
        else 
            cout << "Unable to open file";
    outfile.close();
    }
    else
        cout << "Unable to open file";

    return 0;
}