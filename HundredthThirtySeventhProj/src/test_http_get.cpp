#include "iclient.h"

void test_http_get() {
    using namespace ic::client;
    Request request("http://httpbin.org/ip");
    Response response = request.Perform();

    printf("---------------------------------------------\n");
    printf("%s\n", to_string(response.status()));
    printf("---------------------------------------------\n");
    if (response.status() == Status::SUCCESS
        && response.http_status_code() == http::StatusCode::HTTP_200_OK)   // equal to response.ok()
    {
        // Header
        printf("------ Headers -------------------------------\n");
        auto& headers = response.headers();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
        // Data
        printf("------ Body ----------------------------------\n");
        printf("%s\n", response.data().c_str());
        printf("---------------------------------------------\n");
    }
    printf("\n\n");
}
