class CUtlString {
public:
	const char* Get() const {
		if (!m_pString)
			return "";
		return m_pString;
	}

private:
	char* m_pString;
};
