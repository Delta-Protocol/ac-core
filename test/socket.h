#ifndef TEST_SOCKET_FUNCTION_H  
#define TEST_SOCKET_FUNCTION_H



struct datagram {

		datagram(uint16_t service);
		datagram(uint16_t service, uint16_t);
		datagram(uint16_t service, const string&);
		datagram(uint16_t service, vector<uint8_t>&&) {}

		bool completed() const;
		uint32_t decode_size() const;
		uint16_t decode_service() const;
		void encode_size(uint32_t);
		void encode_service(uint16_t);
		
		hash_t compute_hash() const;

		vector<string> parse_strings() const;
		string parse_string() const;

};

#endif
