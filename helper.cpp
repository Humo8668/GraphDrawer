#include "helper.h"
#include <math.h>

char digitToChar(int digit)
{
	return (digit<=9 && digit>=0)?digit + '0':'\0';
}

char* intToStr(int a)
{
	char *res = NULL;
	char sgn = (a>0)?1:((a==0)?0:-1), nonzero_found = 0, temp; 
	int tenPower, i, j, currDigit;
	
	tenPower = 1000000000; // Max_int ~ 2 000 000 000

	//res = (char*)malloc(10 + (sgn==-1)?1:0); // 9 digits + zeroCode<\0> + sign
	res = (char*)malloc(11); // 9 digits + zeroCode<\0> + sign

	j = 0;
	if(sgn == -1) 
	{
		res[0] = '-';
		j = 1;
	}
	
	
	for(i = 9; i >= 0; i--) // Max_int ~ 2 000 000 000
	{
		currDigit = (sgn*(int)(a/tenPower))%10;
		if(currDigit != 0 && !nonzero_found)
			nonzero_found = 1;
		
		if(nonzero_found)
		{
			temp = digitToChar(currDigit);
			if(temp == '\0')
				return 0;
			res[j] = temp;
			j++;
		}
		
		tenPower /= 10;
	}
	if(!nonzero_found)
	{
		res[j] = '0';
		res[j+1] = '\0';
	}
	else
		res[j] = '\0';
	
	return res;
}

char* dblToStr(double a, int precision)
{
	int whole, fractional, whole_len, fract_len, i, j;
	char *str, *whole_str, *fract_str, sign;
	whole = (int)a;
	sign = (a>0)?1:(a<0)?-1:0;
	a = sign*a;
	/*if(whole == -2147483648) // int overflow
		return 0;*/
	if(precision > 9 || precision < 1)
		precision = 9;
	
	fractional = (int)(pow(10.0, precision)*(a - sign*whole));
	whole_str = intToStr(sign*whole);
	fract_str = intToStr(fractional);
	
	for(whole_len = 0; whole_len < 10; whole_len++)
		if(whole_str[whole_len] == '\0')
			break;
			
	for(fract_len = 0; fract_len < 10; fract_len++)
		if(fract_str[fract_len] == '\0')
			break;
					
	//str = (char*)malloc((whole_len + 1 + precision + 1 + (sign==-1)?1:0)*sizeof(char));
	str = (char*)malloc(whole_len + 1 + precision + 1 + 1);

	j = 0;
	if(sign==-1)
	{
		str[0] = '-';
		j = 1;
	}
	for(i = j; i < whole_len + j; i++)
		str[i] = whole_str[i-j];
	
	str[whole_len+j] = '.';
		
	for(i = whole_len+1 +j; i < whole_len + 1 + (precision - fract_len)+1 +j; i++) //  precision - fract_len  -- count of zeroes before nonzero
	{
		str[i] = '0';
	}
	
	for(i = whole_len + 1 + (precision - fract_len) +j; i < whole_len + 1 + precision +j; i++)
		str[i] = fract_str[i-(whole_len + 1 + (precision - fract_len) +j)];
	
	str[i] = '\0';
	
	return str;
}

int StrLength(const char* str)
{
	int i = 0;
	while(str[i] != '\0' && i < INT_MAX)
		i++;
		
	return i;
}