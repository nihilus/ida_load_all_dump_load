unsigned long htodw(char * hex_string)
{
	unsigned long ret_value = 0;
	unsigned char * hex;

	hex = (unsigned char *)hex_string;

	for (unsigned long i = 0; i < 8; i++)
	{
		if (hex[i] == 0)
			break;
		
		ret_value = ret_value << 4;

		if (hex[i] >= '0' && hex[i] <= '9')
		{
			ret_value += hex[i] - '0';
			continue;
		}

		if (hex[i] >= 'a' && hex[i] <= 'f')
		{
			ret_value += hex[i] - 'a' + 10;
			continue;
		}

		if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			ret_value += hex[i] - 'A' + 10;
			continue;
		}
	}


	return ret_value;

}