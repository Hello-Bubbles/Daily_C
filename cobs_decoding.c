#include<stdio.h>
#include<stdint.h>



uint16_t ascii_int(uint16_t num)
{
    if(num>47 && num<58)
       return num-48;
    if(num>96 &&num<103)
       return num-87;

}
int whole_num(int16_t bytes,FILE *fp)
{
   int16_t ch;
   ch= getc(fp);
      bytes=(ascii_int((uint16_t)bytes)<<4)|(ascii_int((uint16_t)ch));
   return bytes;
}

int main()
{ 

    FILE *fp1,*fp2;
    int16_t length,bytes;
    fp1=fopen("encoded.txt","r");
    fp2=fopen("decoded.txt","w");
    if(fp1==NULL)
     {

        printf("files doesn't exist");
        return -1;
      }

      length=getc(fp1);
    while(length!=EOF && length !='\n')
    {
         length=whole_num(length,fp1);
         printf("length:%d\n",length);
        
         if(length>255)
               printf(" length of frame is greater than 255 cannot process");
         if(length!=0x01)
         {
              
               
        
        length=(length)*2;
         printf("updated length:%d\n",length);
        if(length!=0)
        {
       while((length--)!=2 )
        { 
        
        bytes=getc(fp1);
        fputc(bytes,fp2);
       //bytes= whole_num(bytes,fp1);
        printf("bytes:%d \n", bytes); 
        
        
         }  
         }
         }
         else
         {
          fputc(48,fp2);
           fputc(48,fp2);
          
         }
        length=getc(fp1);
        
        
     }
}
