# sms_client
an async client for sms platform, header only and c++17 is needed.

# quick example

	//the sms template definition is: std::string TEMPLATE_TEXT = u8"【xx科技】您的验证码是" in cons.h
	//notice: the text must be utf8
	//本示例是以云片短信平台例子，你也可以很容易地修改为其他的短信平台，在cons.h中修改参数即可

	std::string appkey = "23ea67fdc73f9aa82k83b99dct144c5f"; 
	std::string phone = "12345678900";
	std::string code = "123456";

	client_factory& factory = client_factory::instance();
	for (size_t i = 0; i < 2; i++) {
		auto client = factory.new_client();
		client->send(appkey, phone, code);
	}

  	factory.run();
