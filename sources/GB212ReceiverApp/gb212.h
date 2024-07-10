#ifndef GB212_H
#define GB212_H
#include "type.h"
#include "func.h"

// ���� 212
// ���е�ͨѶ��������ASCII��
class GB212
{
public:
    typedef std::vector<GB212> GB212Array;
    typedef std::vector<StringMap> DataItemArray;
    // �з�����
    static void split_kv(DataItemArray& _return, const String& cp) {
        auto arr1 = Math::Tools::split(cp, ";", true);
        for (auto& i : arr1) {
            StringMap item;
            auto arr2 = Math::Tools::split(i, ",", true);
            for (auto& j : arr2) {
                auto arrkv = Math::Tools::split(j, "=", false);
                if (arrkv.size() == 2) {
                    item.insert(std::make_pair(arrkv[0], arrkv[1]));
                }
            }
            _return.emplace_back(item);
        }
    }
    // �������
    static String join_kv(const DataItemArray& arr) {
        StringArray item;
        for (auto& i : arr) {
            StringArray arrkv;
            for (auto& j : i) {
                arrkv.emplace_back(j.first + "=" + j.second);
            }
            item.emplace_back(Math::Tools::join(arrkv, ","));
        }
        return Math::Tools::join(item, ";");
    }
    // ������
    struct DataCP {
        DataCP() {}
        DataCP(const String& s) {
            DataItemArray arr;
            split_kv(arr, s);
            for (auto& i : arr) {
                auto kvlen = i.size();
                String key;
                for (auto& j : i) {
                    // ��ָ��������ӵ��ͳһʹ�����Ӵ���
                    if (j.first == "PolId") {
                        key = j.second;
                        this->Value[key] = StringMap();
                        continue;
                    }
                    String name, field;
                    // ��ѯ�Ƿ������׼Э���е��豸״̬
                    auto f1 = j.first.find("SB");
                    auto f2 = j.first.find("-");
                    if (f2 != String::npos && f1 != 0) {
                        // a20004-Rtd   name-field
                        name = j.first.substr(0, f2);
                        field = j.first.substr(f2 + 1);
                        // i11001-Info   field
                        if (field == "Info") {
                            field = name;
                        }
                        else {
                            key = name;
                        }
                    }
                    else {
                        if (j.first == "DataTime") {
                            this->DataTime = j.second;
                        }
                        name = j.first;
                        key = name;
                        field = "value";
                    }
                    this->Value[key].insert(std::make_pair(field, j.second));
                }
            }
            // ���������DataTime�ֶΣ��򽫵�ǰʱ����Ϊ����ʱ��
            if (this->Value.find("DataTime") == this->Value.end()) {
                this->DataTime = Math::Date::getnow("%04d%02d%02d%02d%02d%02d");
            }
        }
        void clear() {
            this->Value.clear();
        }
        std::map<String, StringMap> Value;
        String DataTime;
    };
    // ���ݶ�
    struct Data {
        String QN;		// ������� 20�ַ�	QN=YYYYMMDDHHmmssZZZ
        String ST;		// ϵͳ���� 5�ַ�		ST=21
        String CN;		// ������� 7�ַ�		CN=2011
        String PW;		// �������� 9�ַ�		PW=123456
        String MN;		// �豸��ʶ 27�ַ�	MN=[0-9A-F]
        String Flag = "4";	// ��־λ   8����		Flag=7	bit:000001(Э��汾)0(�Ƿ��а���)0(�Ƿ���Ӧ��)
        String PNUM;	// �ܰ���   9�ַ�		PNUM=0000	[���ְ���û�б��ֶ�]
        String PNO;		// ����     8�ַ�	PNO=0000	[���ְ���û�б��ֶ�]
        String CP;		// ָ�����	<=950�ַ�	CP=&&������&&
        DataCP CPs;
        // ����flag, bit��1��ʼ
        void set_flag(uint32 bit, bool enable) {
            int32 flag = 4;
            Math::Tools::to_int(flag, Flag);
            enable ? SETBIT(flag, bit) : CLRBIT(flag, bit);
            this->Flag = std::to_string(flag);
        }
        // ��ȡflag, bit��1��ʼ
        bool get_flag(uint32 bit) const {
            int32 flag = 4;
            Math::Tools::to_int(flag, Flag);
            return GETBIT(flag, bit);
        }
        // ��Ч��
        bool valid() const {
            return bvalid;
        }
        // ����
        size_t size() const {
#define ADDDataItem(name, subnum) result += name.size() + (name.empty() ? 0 : subnum);
            size_t result = 0;
            ADDDataItem(QN, 3);
            ADDDataItem(ST, 3);
            ADDDataItem(CN, 3);
            ADDDataItem(PW, 3);
            ADDDataItem(MN, 3);
            ADDDataItem(Flag, 5);
            ADDDataItem(PNUM, 5);
            ADDDataItem(PNO, 4);
            ADDDataItem(CP, 3);
#undef ADDDataItem
            return QN.size() + ST.size() + CN.size() + PW.size() + MN.size() + Flag.size() +
                PNUM.size() + PNO.size() + CP.size();
        }
        // ���캯��
        Data() {}
        Data(const String& s){
            CopyStr(s);
        }
        // ��ֵ����
        Data& operator=(const String& str) {
            CopyStr(str);
            return *this;
        }
        void CopyStr(const String& s) {
            auto d1 = s.find("CP=&&");
            auto d2 = s.find("&&", d1 + 5);
            String tmp;
            if (d1 != String::npos && d2 != String::npos) {
                CP = s.substr(d1 + 5, d2 - d1 - 5);
                CPs = CP;
                tmp = s.substr(0, d1) + s.substr(d2 + 2);
            }
            else {
                tmp = s;
            }
            StringMap it;
            auto arr1 = Math::Tools::split(tmp, ";", true);
            for (auto& i : arr1) {
                auto arr2 = Math::Tools::split(i, "=", false);
                if (arr2.size() == 2) {
                    it.insert(std::make_pair(arr2[0], arr2[1]));
                }
            }
#define SETDataItem(name) name = it[#name];
            SETDataItem(QN);
            SETDataItem(ST);
            SETDataItem(CN);
            SETDataItem(PW);
            SETDataItem(MN);
            SETDataItem(Flag);
            SETDataItem(PNUM);
            SETDataItem(PNO);
#undef SETDataItem
            bvalid = true;
        }
        // ��ȡ������
        void CP2Object(DataItemArray& _return) const {
            GB212::split_kv(_return, CP);
        }
        // �ַ������
        String toString() const {
            String str;
#define StrDataItem(name) if (name.size()) str += (#name"=" + name + ";");
            StrDataItem(QN);
            StrDataItem(ST);
            StrDataItem(CN);
            StrDataItem(PW);
            StrDataItem(MN);
            StrDataItem(Flag);
            StrDataItem(PNUM);
            StrDataItem(PNO);
#undef StrDataItem
            str += ("CP=&&" + CP + "&&");
            return str;
        }

    private:
        bool bvalid = false;
    };
    String	header = "##";		// ��ͷ	2�ַ�
    String	datalen = "0000";	// ���ݶγ���		4�������糤100��дΪ"0100"
    Data	data;				// ���ݶ�	<=1024
    String	crc = "0000";		// CRCУ��	4hex
    String	tailer = "\r\n";	// ��β	2�ַ�

    String	full;	// ȫ����

    // ���
    String toString() const {
        auto datastr = data.toString();
        uint16 jisuan_crc16 = Math::Byte::crc16_checksum((uint8*)datastr.data(), datastr.size());
        char out[2048];
        sprintf(out, "##%04d%s%04X\r\n", datastr.size(), datastr.c_str(), jisuan_crc16);
        return out;
    }
    // ��Ч��
    bool valid() const {
        return bvalid;
    }
    // ��Ч��
    bool is_data() const {
        return this->data.CN == "2011" || this->data.CN == "2051" || this->data.CN == "2061"
            || this->data.CN == "2031" || this->data.CN == "2041" || this->data.CN == "3020";
    }
    // ����
    size_t size() const {
        return header.size() + datalen.size() + data.size() + crc.size() + tailer.size();
    }
    // ���������
    void clear_cp() {
        this->data.CP.clear();
        this->data.CPs.clear();
    }
    // �����Ƿ���Ӧ��
    void set_need_reply(bool need) {
        data.set_flag(1, need);
    }
    // �����Ƿ��а���
    void set_need_subpack(bool need) {
        data.set_flag(2, need);
        if (!need) {
            data.PNUM.clear();
            data.PNO.clear();
        }
    }
    // �Ƿ���Ӧ��
    bool is_need_reply() const {
        return data.get_flag(1);
    }
    // �Ƿ��а���
    bool is_need_subpack() {
        return data.get_flag(2);
    }

    // ���������ֶ�����ֵ�á�=�����ӣ�
    // ���������У�ͬһ��Ŀ�Ĳ�ͬ����ֵ���á�,�����ָ�����ͬ��Ŀ֮�� �á�;�����ָ���
    void combine() {
        this->datalen = Math::Tools::to_string("%04d", this->data.size());
        auto datastr = this->data.toString();
        uint16 jisuan_crc = Math::Byte::crc16_checksum((uint8*)datastr.data(), datastr.size());
        this->crc = Math::Tools::to_string("%04X", jisuan_crc);
        this->bvalid = true;
    }

private:
    GB212(const String& str) {
        this->full = str;
        size_t total_size = str.size();
        datalen = str.substr(header.size(), datalen.size());
        int32 data_len = 0;
        Math::Tools::to_int(data_len, datalen);
        if ((10 + data_len) > total_size/* || data_len == 0*/) {
            return;
        }
        auto datastr = str.substr(header.size() + datalen.size(), data_len);
        data = datastr;
        crc = str.substr(header.size() + datalen.size() + data_len, crc.size());
        uint16 jisuan_crc16 = Math::Byte::crc16_checksum((uint8*)datastr.data(), datastr.size());
        int32 src_crc16;
        if (!Math::Tools::to_int(src_crc16, crc, 16)) {
            return;
        }
        if (src_crc16 != jisuan_crc16) {
            return;
        }
        bvalid = true;
    }

    bool bvalid = false;

public:
    GB212(const GB212& r) {
        this->header = r.header;
        this->datalen = r.datalen;
        this->data = r.data;
        this->crc = r.crc;
        this->tailer = r.tailer;
        this->full = r.full;
        this->bvalid = r.bvalid;
    }
    GB212(const String& st, const String& cn, const String& mn, const String& pw, const String& cp, bool need_reply) {
        this->data.QN = Math::Date::getnow("%04d%02d%02d%02d%02d%02d000");
        this->data.ST = st;
        this->data.CN = cn;
        this->data.MN = mn;
        this->data.PW = pw;
        this->data.CP = cp;
        this->set_need_reply(need_reply);
        this->combine();
    }
    // ��ȡ��Ӧ����
    GB212 data_res(const char* cn) const {
        GB212 out(*this);
        out.data.ST = "91";
        out.data.CN = cn;
        out.clear_cp();
        out.combine();;
        return out;
    }
    // ����
    static void Parser(GB212Array& _return, const String& buffer) {
        auto buffsize = buffer.size();
// 		if (buffsize < 10) {
// 			return;
// 		}
#if 1
        String it;
        for (int i = 0; i < buffsize; i++)
        {
            if (i < buffsize - 1 && buffer[i] == '#' && buffer[i + 1] == '#') {
                if (it.size()) {
                    _return.emplace_back(GB212(it));
                    it.clear();
                }
            }
            it += (char)buffer[i];
            if (i > 0 && buffer[i - 1] == '\r' && buffer[i] == '\n') {
                if (it.size()) {
                    _return.emplace_back(GB212(it));
                    it.clear();
                }
            }
        }
        if (it.size()) {
            _return.emplace_back(GB212(it));
            it.clear();
        }
#else
        size_t npos = 0;
        int32 loopcount = 0;
        do
        {
            auto h = buffer.find("##", npos);
            auto t = buffer.find("\r\n", h);
            if (h != String::npos) {
                if (t == String::npos) {
                    t = buffer.find("##", h + 2);
                }
                else {
                    t += 2;
                }
                //auto ss = buffer.substr(h, t - h);
                _return.emplace_back(GB212(buffer.substr(h, t - h)));
            }
            else {
                h = npos;
                t = buffsize;
                _return.emplace_back(GB212(buffer.substr(h, t - h)));
            }
            npos = t;
            if (++loopcount > 20) {
                printf("GB212:Parser While Exception\r\n");
            }
        } while (npos < buffsize);
#endif
    }
    // ����
    static void New() {

    }
};

#endif // GB212_H
