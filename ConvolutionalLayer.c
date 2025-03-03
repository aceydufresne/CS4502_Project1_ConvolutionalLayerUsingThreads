#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

pthread_mutex_t x = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t y = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
unsigned char *quadrant1;
unsigned char *quadrant2;
unsigned char *quadrant3;
unsigned char *quadrant4;
int width, height, channels;
float threshold;
}threadData;


typedef struct{
int width, height, channels;
unsigned char *img;
float threshold;
}Image;

int IPC();
void toGrey(Image *imgData);
void quadrantMaker(Image *imgData, threadData *threadInfo);
void Finished(threadData *data);
void *computation(void *arg);
void *computation1(void *arg);
void *computation2(void *arg);
void *computation3(void *arg);

int main(){
int height, width, channels;
char fileName[20];
printf("Enter an Image file pathway:");
//CNNCharacterTest.jpg
//CNNCharacterTest2.png

scanf("%s", fileName);
unsigned char *img = stbi_load(fileName, &width, &height, &channels, 0);
if(img == NULL){
printf("Image Error");
return 1;
}
if(channels < 3){
printf("Error for Color Channels");
return 1;
}

Image kevin;
kevin.width = width;
kevin.height = height;
kevin.channels = channels;
kevin.img = img;
toGrey(&kevin);

threadData threadInfo;
quadrantMaker(&kevin,&threadInfo);
Finished(&threadInfo);
stbi_image_free(img);
IPC();
return 0;
}


void toGrey(Image *kevin){
    int width = kevin->width;
    int height = kevin->height;
    int channels = kevin->channels;
    threadData data;
    unsigned char *grey = malloc(width * height * sizeof(unsigned char));

for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            int index = i * width * channels + j * channels;
            grey[i * width + j] = (unsigned char)(0.27 * kevin->img[index] +0.72 * kevin->img[index + 1]+0.07 * kevin->img[index + 2]);
        }
    }

    int length = width * height;
    float sum = 0;
    for(int i = 0; i < length; i++)
    {
        sum = grey[i] + sum;
    }
   
kevin->threshold = sum/length;
kevin->channels = 1;
kevin->img = grey;
data.threshold = kevin->threshold;

return;
}


void quadrantMaker(Image *imgData, threadData *threadInfo){


    int width = imgData->width;
    int height = imgData->height;
    int quadWidth = width / 2;
    int quadHeight = height / 2;
   
    threadInfo->width = quadWidth;
    threadInfo->height = quadHeight;
    threadInfo->channels = 1;
    threadInfo->threshold = imgData->threshold;

    threadInfo->quadrant1 = malloc(quadWidth * quadHeight * sizeof(unsigned char));
    threadInfo->quadrant2 = malloc(quadWidth * quadHeight * sizeof(unsigned char));
    threadInfo->quadrant3 = malloc(quadWidth * quadHeight * sizeof(unsigned char));
    threadInfo->quadrant4 = malloc(quadWidth * quadHeight * sizeof(unsigned char));
pthread_t threads[4];

for (int i = 0; i < quadHeight; i++){
        for (int j = 0; j < quadWidth; j++){
            int srcIndex = i * width + j;
            int destIndex = i * quadWidth + j;
            threadInfo->quadrant1[destIndex] = imgData->img[srcIndex];
        }
    }
    for (int i = 0; i < quadHeight; i++){
        for (int j = quadWidth; j < width; j++){
            int srcIndex = i * width + j;
            int destIndex = i * quadWidth + (j - quadWidth);
            threadInfo->quadrant2[destIndex] = imgData->img[srcIndex];
        }
    }
    for (int i = quadHeight; i < height; i++){
        for (int j = 0; j < quadWidth; j++){
            int srcIndex = i * width + j;
            int destIndex = (i - quadHeight) * quadWidth + j;
            threadInfo->quadrant3[destIndex] = imgData->img[srcIndex];
        }
    }
    for (int i = quadHeight; i < height; i++){
        for (int j = quadWidth; j < width; j++){
            int srcIndex = i * width + j;
            int destIndex = (i - quadHeight) * quadWidth + (j - quadWidth);
            threadInfo->quadrant4[destIndex] = imgData->img[srcIndex];
        }
    }


pthread_create(&threads[0], NULL, computation, (void *)threadInfo);
pthread_create(&threads[1], NULL, computation1, (void *)threadInfo);
pthread_create(&threads[2], NULL, computation2, (void *)threadInfo);
pthread_create(&threads[3], NULL, computation3, (void *)threadInfo);

for(int i = 0; i < 4; i++){
        pthread_join(threads[i], NULL);
    }
    

    printf("Calling Processes.");

    return;
}


void *computation(void *arg){
    pthread_mutex_lock(&x);
    pthread_mutex_lock(&y);

    threadData *data = (threadData *)arg;
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            int index = i * data->width + j;
            if (data->quadrant1[index] > data->threshold) {
                data->quadrant1[index] = 0;
            }
        }
    } 
    pthread_mutex_unlock(&y);
    pthread_mutex_unlock(&x);
    return NULL;
}
void *computation1(void *arg){
pthread_mutex_lock(&x);
    pthread_mutex_lock(&y);

    threadData *data = (threadData *)arg;
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            int index = i * data->width + j;
            if (data->quadrant2[index] > data->threshold) {
                data->quadrant2[index] = 0;
            }
        }
    } 
    pthread_mutex_unlock(&y);
    pthread_mutex_unlock(&x);
    return NULL;
}
void *computation2(void *arg){
    pthread_mutex_lock(&x);
    pthread_mutex_lock(&y);

    threadData *data = (threadData *)arg;
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            int index = i * data->width + j;
            if (data->quadrant3[index] > data->threshold) {
                data->quadrant3[index] = 0;
            }
        }
    } 
    pthread_mutex_unlock(&y);
    pthread_mutex_unlock(&x);
    return NULL;
}
void *computation3(void *arg){
pthread_mutex_lock(&x);
    pthread_mutex_lock(&y);

    threadData *data = (threadData *)arg;
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            int index = i * data->width + j;
            if (data->quadrant4[index] > data->threshold) {
                data->quadrant4[index] = 0;
            }
        }
    } 
    pthread_mutex_unlock(&y);
    pthread_mutex_unlock(&x);
    return NULL;
}

void Finished(threadData *data) {
    int quadWidth = data->width;
    int quadHeight = data->height;
    int finalWidth = quadWidth * 2;
    int finalHeight = quadHeight * 2;

    unsigned char *finalImg = malloc(finalWidth * finalHeight * sizeof(unsigned char));
    if (finalImg == NULL) {
        perror("Error");
        exit(1);
    }

    for (int i = 0; i < quadHeight; i++) {
        for (int j = 0; j < quadWidth; j++) {
            int destIndex = i * finalWidth + j;
            int srcIndex  = i * quadWidth + j;
            finalImg[destIndex] = data->quadrant1[srcIndex];
        }
    }

    for (int i = 0; i < quadHeight; i++) {
        for (int j = 0; j < quadWidth; j++) {
            int destIndex = i * finalWidth + (j + quadWidth);
            int srcIndex  = i * quadWidth + j;
            finalImg[destIndex] = data->quadrant2[srcIndex];
        }
    }

    for (int i = 0; i < quadHeight; i++) {
        for (int j = 0; j < quadWidth; j++) {
            int destIndex = (i + quadHeight) * finalWidth + j;
            int srcIndex  = i * quadWidth + j;
            finalImg[destIndex] = data->quadrant3[srcIndex];
        }
    }

    for (int i = 0; i < quadHeight; i++) {
        for (int j = 0; j < quadWidth; j++) {
            int destIndex = (i + quadHeight) * finalWidth + (j + quadWidth);
            int srcIndex  = i * quadWidth + j;
            finalImg[destIndex] = data->quadrant4[srcIndex];
        }
    }
    //printf("This is the new code");
    stbi_write_png("final.png", finalWidth, finalHeight, 1, finalImg, finalWidth);

    free(finalImg);
}

int IPC(){
    //IPC Implementation
    //int fd[2];
    //0 read, 1 write
    //if(pipe(fd) == -1){
    //    printf("IPC Error.");
    //    return 1;
    //}
    //int id = fork();
    //if(id == 0){
        //child
    //    close(fd[0]);
     //   int xd;
    //    printf("Insert an integer:");
    //    scanf("%d", &&xd);
    //    if(write(fd[1], &xd, sizeof(int)) == -1){
    //        printf("Error occurred within child.");
   //         return 2;
    //    }
        
    //    close(fd[1]);
   // }else{
        //parent
     //   close(fd[1]);
     //   int yd;
     //   if(read(fd[0], &yd, sizeof(int)) == -1){
    //        printf("Error occurred within parent.");
    //        return 3;
    //    }
    //    close(fd[0]);
    //    printf("Got from child process %d\n", yd);

   // }
    //End IPC Pipe
 return 0;
}