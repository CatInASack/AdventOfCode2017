#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode2017
{		
	TEST_CLASS(Day1_1)
	{
    private:
        int Captcha(const std::string& input)
        {
            auto total = 0;

            for (auto current = size_t(0); current < input.length(); current++)
            {
                auto next = (current + 1) % input.length();

                if (input[current] == input[next])
                {
                    total += input[current] - '0';
                }
            }

            return total;
        }

	public:
		
		TEST_METHOD(Day1_1_Test1)
		{
            Assert::AreEqual(3, Captcha("1122"));
		}

        TEST_METHOD(Day1_1_Test2)
        {
            Assert::AreEqual(4, Captcha("1111"));
        }
    
        TEST_METHOD(Day1_1_Test3)
        {
            Assert::AreEqual(0, Captcha("1234"));
        }
    
        TEST_METHOD(Day1_1_Test4)
        {
            Assert::AreEqual(9, Captcha("91212129"));
        }

        TEST_METHOD(Day1_1_Final)
        {
            Assert::AreEqual(1097, Captcha(
                "818275977931166178424892653779931342156567268946849597948944"
                "469863818248114327524824136924486891794739281668741616818614"
                "613222585132742386168687517939432911753846817997473555693821"
                "316918473474459788714917665794336753628836231159578734813485"
                "687247273288926216976992516314415836985611354682821892793983"
                "922755395577592859959966574329787693934242233159947846757279"
                "523939217844194346599494858459582798326799512571365294673978"
                "955928416955127211624234143497546729348687844317864243859238"
                "665326784414349618985832259224761857371389133635711819476969"
                "854584123589566163491796442167815899539788237118339218699137"
                "497532932492226948892362554937381497389469981346971998271644"
                "362944839883953967698665427314592438958181697639594631142991"
                "156327257413186621923369632466918836951277519421695264986942"
                "261781256412377711245825379412978876134267384793694756732246"
                "799739464721215446477972737883445615664755923441441781128933"
                "369585655925615257548499628878242122434979197969569971961379"
                "367756499884537433839217835728263798431874654317137955175565"
                "253555735968376115749641527957935691487965161211853476747758"
                "982854811367422656321836839326818976668191525884763294465366"
                "151349347633968321457954152621175837754723675485348339261288"
                "195865348545793575843874731785852718281311481217515834822185"
                "477982342271937155479432673815629144664144538221768992733498"
                "856934255518875381672342521819499939835919827166318715849161"
                "715775427981485233467222586764392783699273452228728667175488"
                "552924399518855743923659815483988899924199449721321589476864"
                "161778841352853573584489497263216627369841455165476954483715"
                "112127465311353411346132671561568444626828453687183385215975"
                "319858714144975174516356117245993696521941589168394574287785"
                "233685284294357548156487538175462176268162852746996633977948"
                "755296869616778577327951858348313582783675149343562362974553"
                "976147259225311183729415381527435926224781181987111454447371"
                "894645359797229493458443522549386769845742557644349554641538"
                "488252581267341635761715674381775778868374988451463624332123"
                "361576518411234438681171864923916896987836734129295354684962"
                "897616358722633724198278552339794629939574841672355699222747"
                "886785616814449297817352118452284785694551841431869545321438"
                "468118"));
        }
    };
}