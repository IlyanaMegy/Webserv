#ifndef SINGLETON
#define SINGLETON

template <typename X>
class Singleton {
   public:
	static X& getInstance() {
		static X instance;
		return instance;
	}
	Singleton() {}
	Singleton(Singleton const&) {};
	~Singleton() {};
	Singleton& operator=(const Singleton& src) {(void)src; return *this; };

   private:
};

#endif
