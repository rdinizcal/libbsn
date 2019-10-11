#include "processor/Processor.hpp"

using namespace std;

namespace bsn {
    namespace processor {
        // Retorna o id baseado no tipo
        int32_t get_sensor_id(string type) {
            if (type == "thermometer")
                return 0;
            else if (type == "ecg")
                return 1;
            else if (type == "oximeter")
                return 2;
            else if (type == "bpms")
                return 3;
            else if (type == "bpmd")		
                return 4;
            else if (type == "acc")		
                return 5;
            else {
                cout << "UNKNOWN TYPE " + type + '\n';
                return -1;
            }

        }

        double get_value(string packet){
            double ret = stod(packet.substr(packet.find('-')+1,packet.length()));
            return ret;
        }

        // Retorna true se as tres listas são não vazias
        bool available_to_process(vector<list<double>> packetsReceived){
            bool available = false;

            for(auto packet_list : packetsReceived){
                if(!packet_list.empty()){
                    available = true;
                    break;
                }
            }

            return available;
        }

        double data_fuse(vector<list<double>> &packetsReceived) {	
            double average, risk_status;
            int32_t count = 0;
            average = 0;

            // Se não existiver disponível não processa
            if(!available_to_process(packetsReceived))
                return -1;
            
            std::vector<double> values;

            for(auto &packet_list : packetsReceived){
                if(!packet_list.empty()) {
                    // Soma à média e retira da fila
                    average += packet_list.front();
                    values.push_back(packet_list.front());
                    // Descarta o pacote processado se existem
                    // Mais outros para serem processados
                    if(packet_list.size() > 1) {
                        packet_list.pop_front();
                    }			
                    count++;
                }
            }

            // Calcula a media partir da soma dividida pelo número de pacotes lidos
            double avg = (average / count);

            std::vector<double> deviations;
            double min = 1000; //Maior valor possível é 100
            double max = -1; //Menor valor possível é 0

            size_t i;

            for(i = 0;i < values.size();i++) {
                //Cálculo dos desvios individuais
                double dev;
                dev = values.at(i) - avg;

                deviations.push_back(dev);

                if(dev > max) {
                    max = dev;
                } else if(dev < min) {
                    min = dev;
                }
            }

            double weighted_average = 0.0;
            double weight_sum = 0.0;

            // Status de risco do paciente dado em porcentagem
            if(max - min > 0) {
                //Se o máximo e mínimo forem diferentes, normalizar desvios e calcular média ponderada
                for(i = 0;i < deviations.size();i++) {
                    //Normalizando desvios entre 0 e 1
                    deviations.at(i) = (deviations.at(i) - min)/(max - min);

                    weight_sum += deviations.at(i);
                    weighted_average += values.at(i)*deviations.at(i);
                }

                risk_status = weighted_average/weight_sum;
            } else {
                //Se o máximo é igual ao mínimo, a média será calculada e dará o mesmo valor
                risk_status = avg / count;
            }

            // 85.0 é um número totalmente ARBITRARIO
            if(risk_status > 66.0){
                cout << "============ EMERGENCY ============(" << risk_status << '%' << ")" << endl;
            }
            else{
                cout << "General risk status: " << risk_status << '%' << endl;
            }

            return risk_status;
        }
    }
}