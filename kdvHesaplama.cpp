#include <iostream>
#include <curl/curl.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

double getExchangeRate(const string& apiUrl) {
    CURL* curl = curl_easy_init();
    string response_data;

    if (curl) { 
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == CURLE_OK) {
            try {
                auto json_data = json::parse(response_data);
                if (json_data.contains("rates") && json_data["rates"].contains("TRY")) {
                    return json_data["rates"]["TRY"].get<double>();
                } else {
                    cerr << "Kur bilgisi bulunamadı." << endl;
                    return 0.0;
                }
            } catch (const json::exception& e) {
                cerr << "Ayrıştırma hatası : " << e.what() << endl;
                return 0.0;
            }
        } else {
            cerr << "HTTP isteği başarısız : " << curl_easy_strerror(res) << endl;
            return 0.0;
        }
    }
    return 0.0;
}

int main() {
    using namespace std;
    system("chcp 65001 > nul");
    const string apiUrl = "https://api.frankfurter.app/latest?from=USD&to=TRY";
    double kurOran = getExchangeRate(apiUrl);
    if (kurOran <= 0.0) {
        cerr << "Hata : Dolar kuru alınamadı, Program sonlandırılıyor." << endl;
        return 1;
    }

    cout << "Güncel Dolar kuru : " << kurOran << " TL" << endl;
    cout << "################################################" << endl;

    double fiyat = 0.0;
    double kdvOran = 0.0;
    double tutar = 0.0;

    cout << "Fiyat ($) : ";
    cin >> fiyat;

    cout << "KDV Orani (%) : ";
    cin >> kdvOran;

    double dolarFiyat = fiyat * kurOran;
    double kdvliTutar = dolarFiyat + (dolarFiyat * kdvOran / 100.0);

    cout << "Toplam tutar : " << kdvliTutar << " TL" << endl;

    system("pause");
    return 0;
}