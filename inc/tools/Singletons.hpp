#ifndef SINGLETON
#define SINGLETON

/**
 * @brief	design pattern that restricts the instantiation of a class
 * to one object.
 * @note	Debugger instance everywhere.
 */
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
