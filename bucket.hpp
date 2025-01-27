#include <Arduino.h>

class Bucket
{
    private:
        uint32_t _bucket_size;
        uint32_t _available_tokens = 0;
        uint32_t _token_refill_ms;

        uint32_t _last_request_time;

        void _refill_tokens()
        {
            uint32_t current_time = millis();
            uint32_t elapsed_time = current_time - _last_request_time;

            uint32_t tokens_to_add = elapsed_time / _token_refill_ms;

            _available_tokens = min(_available_tokens + tokens_to_add, _bucket_size);
        }

    public:
        Bucket(uint32_t bucket_size) : Bucket(bucket_size, 20){};

        Bucket(uint32_t bucket_size, uint32_t token_refill_ms)
            : _bucket_size(bucket_size), _token_refill_ms(token_refill_ms), _last_request_time(millis()){};

        ~Bucket(){};

        void request_token()
        {
            _refill_tokens();
            while (_available_tokens <= 0) {
                _refill_tokens();
            }
            _last_request_time = millis();
            _available_tokens -= 1;
        }
};
