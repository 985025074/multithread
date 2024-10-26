#include "FormatTools.h"
namespace syc{
    namespace details{
inline std::string_view get_filename(std::string_view filename){
    auto loc = filename.rfind('/');
    return std::string_view(filename.begin()+loc+1,filename.end());
}
inline std::string get_gmt_time(){
    std::time_t currentTime = std::time(nullptr);
    std::tm* gmtTime = std::gmtime(&currentTime);
    std::string gmtTimeStr(std::asctime(gmtTime));
    return gmtTimeStr;

}   
    };
};