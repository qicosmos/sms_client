#include <iostream>
#include "client_factory.hpp"

int main() {
	//the sms template definition is: std::string TEMPLATE_TEXT = u8"��xx�Ƽ���������֤����" in cons.h
	//notice: the text must be utf8
	//��ʾ��������Ƭ����ƽ̨���ӣ���Ҳ���Ժ����׵��޸�Ϊ�����Ķ���ƽ̨����cons.h���޸Ĳ�������

	std::string appkey = "23ea67fdc73f9aa82k83b99dct144c5f"; 
	std::string phone = "12345678900";
	std::string code = "123456";

	client_factory& factory = client_factory::instance();
	for (size_t i = 0; i < 2; i++) {
		auto client = factory.new_client();
		client->send(appkey, phone, code);
	}

	factory.run();

	return 0;
}