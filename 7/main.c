// main.c

void main()
{
	int line = 5;
	char str[11] = "HelloWorld";

	char *video = (char*)(0xB8000 + 160 * line);

	for (int i = 0; str[i] != 0; i++)
	{
		*video++ = str[i];
		*video++ = 0x03; // 첫 째는 배경색, 둘 때는 글자 자체 색
	}

	return;
}