#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <SDL2/SDL.h>

void paintPolygonArea(SDL_Renderer *renderer, int *koordinat, int firstPoint, int lastPoint, int scaleFactor) {
    // Koordinatları saklamak için bir dizi oluştur
    int x[lastPoint - firstPoint];
    int y[lastPoint - firstPoint];
    printf("Control00");
    // x ve y değerlerini ayır
    for (int i = 0; i < (lastPoint - firstPoint)/2; i++) {
        x[i] = koordinat[firstPoint + i * 2] * scaleFactor;
        y[i] = koordinat[firstPoint + i * 2 + 1] * scaleFactor;
        printf("Control1 %d %d\n",x[i],y[i]);
    }
    printf("Control2");
    // Dizideki en küçük x ve y değerlerini bul
    int minX = x[0], minY = y[0];
    for (int i = 1; i < (lastPoint - firstPoint)/2; i++) {
        if (x[i] < minX) minX = x[i];
        if (y[i] < minY) minY = y[i];
    }
    printf("Control3");
    // Dizideki en büyük x ve y değerlerini bul
    int maxX = x[0], maxY = y[0];
    for (int i = 1; i < (lastPoint - firstPoint)/2; i++) {
        if (x[i] > maxX) maxX = x[i];
        if (y[i] > maxY) maxY = y[i];
    }
    printf("Control4");
    // Kapladığı alanın boyutunu bul
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    printf("Control");
    // Kapladığı alanı boyat
    int alan=0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int testX = minX + i;
            int testY = minY + j;

            // Ray casting algoritması kullanarak test noktası poligonun içinde mi?
            int isInside = 0;
            for (int k = 0; k < (lastPoint - firstPoint)/2; k++) {
                int nextK = (k + 1) % ((lastPoint - firstPoint)/2);
                if (((y[k] > testY) != (y[nextK] > testY)) &&
                    (testX < (x[nextK] - x[k]) * (testY - y[k]) / (y[nextK] - y[k]) + x[k])) {
                    isInside = !isInside;
                }
            }

            if (isInside) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150); // Kırmızı renk, belirli saydamlık
                SDL_Rect rect = {testX, testY, scaleFactor, scaleFactor};
                SDL_RenderFillRect(renderer, &rect);
                alan+=1;
            }
        }
    }
    printf("Sekil1=>Rezerv Alan: %d ",alan);

}
int takeCoordinates(int *koordinat, int numPoints)
{

    int ilkx = koordinat[0], ilky = koordinat[1];
    for(int i; i < numPoints - 2; i += 2)
    {
        int x1 = koordinat[i];
        int y1 = koordinat[i + 1];
        int x2 = koordinat[i + 2];
        int y2 = koordinat[i + 3];

        if(koordinat[i + 2] == ilkx && koordinat[i + 3] == ilky)
        {
            ilkx = koordinat[i + 4];
            ilky = koordinat[i + 5];
            i += 2;
        }
    }
    return 1;
}
void drawGrid(SDL_Renderer *renderer, int numRows, int numCols, int scaleFactor)
{
    // Grid çizmek için döngüler
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);  // Çizimleri saydam hale getir
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 150);

    for (int i = 0; i <= numRows; i++)
    {
        SDL_RenderDrawLine(renderer, 0, i * scaleFactor, numCols * scaleFactor, i * scaleFactor);
    }

    for (int i = 0; i <= numCols; i++)
    {
        SDL_RenderDrawLine(renderer, i * scaleFactor, 0, i * scaleFactor, numRows * scaleFactor);
    }
}
int drawCoordinates(SDL_Renderer *renderer, int *koordinat, int numPoints, int *sorter, int scaleFactor)
{
    int i = 0, j = 0;
    sorter[j] = i;
    int ilkx = koordinat[0], ilky = koordinat[1];
    for ( ; i < numPoints - 2; i += 2)
    {
        int x1 = koordinat[i] * scaleFactor;
        int y1 = koordinat[i + 1] * scaleFactor;
        int x2 = koordinat[i + 2] * scaleFactor;
        int y2 = koordinat[i + 3] * scaleFactor;

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        if(koordinat[i+2] == ilkx && koordinat[i+3] == ilky)
        {
            j++,
            sorter[j] = i + 4;
            ilkx = koordinat[i + 4];
            ilky = koordinat[i + 5];
            i +=2;
        }
    }
    return j;
}

// libcurl'un geri çağırma fonksiyonu
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((char*)userp)[0] = '\0';
    strncat((char*)userp, (char*)contents, size * nmemb);
    return size * nmemb;
}

// URL'den metin çeken fonksiyon
char* UrlDenMetinCek(const char* url)
{
    CURL* curl;
    CURLcode res;
    char* data = (char*)malloc(1024); // Alýnan verinin saklanacaðý bellek alaný

    // libcurl başlatılıyor
    curl = curl_easy_init();

    if(curl)
    {
        // URL'ye istek gönderiliyor
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Veriyi alırken kullanılacak callback fonksiyonu ayarlanıyor
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

        // İstek yapılıyor
        res = curl_easy_perform(curl);

        // Hata kontrolü
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // libcurl temizleniyor
        curl_easy_cleanup(curl);
    }
    return data;
}

int SatirAl(int satir)
{

    printf("Kacinci satirdaki koordinatlari cizelim?:");
    scanf("%d",&satir);
    return satir;
}

int SondajAl(int sondaj)
{
    printf("Birim sondaj maliyeti ne kadar olacaktir?:");
    scanf("%d",&sondaj);
    return sondaj;
}

int PlatformAl(int platform)
{
    printf("Birim platform maliyeti ne kadar olacaktir?:");
    scanf("%d",&platform);
    return platform;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    char* alinanMetin;

    int Satir = SatirAl(Satir);
    printf("%d. satirdaki koordinatlar cizdirilecek.\n",Satir);

    int SondajBrm = SondajAl(SondajBrm);
    int PlatformBrm = PlatformAl(PlatformBrm);

    // URL'yi belirt
    const char* url = "http://bilgisayar.kocaeli.edu.tr/prolab1/prolab1.txt";

    // URL'den metni çek
    alinanMetin = UrlDenMetinCek(url);

    // İlgili satırı al
    char* satirMetni = strtok(alinanMetin, "\n");
    for(int i = 1; i < Satir; i++)
    {
        satirMetni = strtok(NULL, "\n");
    }
    char alinanSatir[100];
    strcpy(alinanSatir,satirMetni);

    //Koordinatları al
    const char ayrac[] = "B(,)F";
    int koordinatlar[100]; // Koordinatları saklayacak dizi

    char *ayir = strtok(satirMetni, ayrac);
    int index = -1; // Koordinatları dizide saklamak için index

    while (ayir != NULL)
    {
        int koordinatDegeri = atoi(ayir);// Koordinatı tamsayıya dönüştür
        koordinatlar[index++] = koordinatDegeri; // Diziye ekle
        ayir = strtok(NULL, ayrac);
    }

    // Koordinatları ekrana yazdır
    for (int i = 0; i < index; i++)
    {
        printf("%d ", koordinatlar[i]);
    }
    int sorter[6];
    int sorterIndex;

    // Bellek temizliği
    free(alinanMetin);

    // Sonuçları görüntüle
    printf("Satir %d: %s\n", Satir, alinanSatir);
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    // Create a window
    SDL_Window *window = SDL_CreateWindow("Grafik Cizimi 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1080, 720, SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Event handling loop
    int quit = 0;
    SDL_Event e;
    int scaleFactor = 10;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
        }
        // Set the color to render (in this case, it's black)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // Clear the screen
        SDL_RenderClear(renderer);

        // İlk olarak ızgarayı çiz
        drawGrid(renderer, 720, 1080, scaleFactor);

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

        sorterIndex = drawCoordinates(renderer, koordinatlar, index, sorter, scaleFactor);
        // Update the screen
        SDL_RenderPresent(renderer);
    }
    // Clean up SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    for(int i=0;i<=sorterIndex ;i++)
    {
        printf("-- %d ",sorter[i]);
    }
    // Yeni pencere oluştur
    SDL_Window *newWindow = SDL_CreateWindow("Yeni Pencere", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1080, 720, SDL_WINDOW_SHOWN);
    SDL_Renderer *newRenderer = SDL_CreateRenderer(newWindow, -1, SDL_RENDERER_ACCELERATED);

    quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
        }
        // Set the color to render (in this case, it's black)
        SDL_SetRenderDrawColor(newRenderer, 0, 0, 0, 255);
        // Clear the screen
        SDL_RenderClear(newRenderer);
      
        for(int i=0 ;i<sorterIndex ;i++)
        {
            paintPolygonArea(newRenderer, koordinatlar, sorter[i], sorter[i+1] , scaleFactor);
        }
        // İlk olarak ızgarayı çiz
        drawGrid(newRenderer, 720, 1080, scaleFactor);

        // Update the screen
        SDL_RenderPresent(newRenderer);
    }
    SDL_DestroyRenderer(newRenderer);
    SDL_DestroyWindow(newWindow);

    SDL_Quit();
    return 0;
}
