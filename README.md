Для работы поисковаго сервиса предоставлены классы  SearchServer ("search_server.h"), RequestQueue ("request_queue.h"), Paginator ("paginator.h").


Сам обьект SearchServer получает, хранит, и осуществляет поиск по документам.
Его конструктор принимает набор желаемых стоп слов по которым не будет осуществлятся поик в документах. Стоп слова могут задаватся одной строкой.  
Метод SearchServer::SetStopWords(const list& set_words) - позволит задать стоп слова после инициализации обькта. шаблонный метод, можно передавть контейнер строк.
Метод void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) - позволит добавить очередной документ
в базу.
document_id - id документа
document - сам документ
status - статус документа
ratings - список рейтингов

Метод std::vector<Document> FindTopDocuments - перегружен, возвретит вектор структур Document() которая имеет поля:
int id - id документа
double relevance - релевантность слова
int rating - райтинг
перегрузки имеют параметры:
raw_query - строка поиска слов, для всех перегрузок
также есть два параметра на выбор, но они не обязательны:
filter_func - функция для сортировки рузультатов запроса(компоратор)
match_status - статус документа, для сортировки 

Метод int GetDocumentCount() const - вернет количество документов.
std:: tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id)  const -
получит строку и id документа и вернет слова найденые в документе и статус.
Метод int SearchServer::GetDocumentId(int index) const - проверит есть ли такой индекс как id документа, если index больше 
количества списка жокументов, то выбросит исключение и вернет -1, иначе вернет id документа.

RequestQueue помогает работать с обьектом SearchServer и принимает его в качестве аргумента конструктора. Нужен для создания и хранения очереди запросов. Сейчас настроен на работу по количеству запросов (1440). 
Метод NewRequest(const std::string& raw_query,const  std::string& filter, std::vector<Document>& list_document) - добавит новый запрос в очередь.
Метод AddFindRequest() - имеет несколько перегрузок,  вернет вектр структур Document() из SearchServer.
Метод GetNoResultRequests() const - вернет количество пустых запросов (те на которые не было ответов)

Также существует класс Paginator (шаблонный) он поможет разбить результаты запросоВ по страницам.
Принимает в свой конструктор шаблонные итераторы на начало и конец списка результатов запросов и количество станиц.
Может кинуть исключение, если количество страниц меньше или равно нулю.
Метод std::vector<std::vector<typename Iterator::value_type>> ReturnList() - вернет вектор векторов (документы для каждой страницы) с указателями на документы.