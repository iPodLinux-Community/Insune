#ifndef _ACTION_H_
#define _ACTION_H_

class ActionBase
{
 public:
	ActionBase() {};
	virtual ~ActionBase() {};
	virtual void callback() const = 0;
};

template <class C, typename T>
class Action : public ActionBase
{
 public:
	typedef void (C::*method_t)(T);

	Action(C *c = 0, method_t m = 0)
	 : m_class(c), m_method(m) {}
	Action(C *c, method_t m, T data)
	 : m_class(c), m_method(m), m_data(data) {}

	void setClass(C *c) { m_class = c; }
	void setMethod(method_t m) { m_method = m; }
	void setData(T d) { m_data = d; }

	void callback() const
	{
		if (m_class && m_method)
			(m_class->*m_method)(m_data);
	}

 private:
	C	*m_class;
	method_t m_method;
	T	 m_data;
};

#endif
