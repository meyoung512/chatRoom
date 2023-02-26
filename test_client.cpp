#include "client.h"

int main(){
    client cli(8023,"127.0.0.1");
    cli.run();
}