#include "input_reader.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
namespace reader
{
    struct Stop
    {
        double _lat = 0.0;
        double _lng = 0.0;
        std::string _stop;
        std::map<std::string, size_t> _distanceToStops;
    };

    struct Bus
    {
        std::string _numberBus;
        std::vector<std::string> _stopBus;
        bool _isCircleOrNot = true;
    };

    Stop ParserStringStop(string& str)
    {
        Stop stop1;

        auto posColon = str.find(":");
        string name = str.substr(5, posColon - 5);
        stop1._stop = name;
        auto posCommaBetweenCoords = str.find(",", posColon);
        string latBuffer = str.substr(posColon + 2, posCommaBetweenCoords - posColon - 2);
        string lngBuffer = str.substr(posCommaBetweenCoords + 2);
        stop1._lat = stod(latBuffer);
        stop1._lng = stod(lngBuffer);

        size_t posCommaAfterCoords = str.find(",",posCommaBetweenCoords + 1);
        while(posCommaAfterCoords != string::npos)
        {
            auto findNextComma = str.find(",", posCommaAfterCoords + 1);
            string disStop = str.substr(posCommaAfterCoords + 2, findNextComma - posCommaAfterCoords - 2);
            auto findMto = disStop.find("m to ");
            size_t distance = stoul(disStop.substr(0, findMto));
            string stop = disStop.substr(findMto + 5);
            stop1._distanceToStops[stop] = distance;
            posCommaAfterCoords = findNextComma;
        }

        return stop1;
    } 

    Bus ParserStringBus(string& str)
    {
        Bus bus1;

        auto posColon = str.find(":");
        string name = str.substr(4, posColon - 4);

        auto posTire = posColon;
        vector<string> AddStops;

        bus1._isCircleOrNot = str.find(">", posColon) != string::npos;

        while (string::npos != posTire)
        {
            posTire = str.find_first_of("->", posColon + 1);
            string stop1 = str.substr(posColon + 2, posTire - posColon - 3);
            posColon = posTire;
            AddStops.push_back(stop1);
        }

        bus1._numberBus = name;
        bus1._stopBus = AddStops;

        return bus1;
    }

    QueryBusStops ParseQueryBus(string& str)
    {
        QueryBusStops qbus;

        auto posBus = str.find(":");
        qbus._name = str.substr(4, posBus - 4);
        return qbus;
    }

    QueryBusStops ParseQueryBusStops(string& str)
    {
        QueryBusStops qstops;
        qstops._isBus = false;

        auto posColon = str.find(":");
        qstops._name = str.substr(5, posColon - 5);
        return qstops;
    }

    void ReadDataQuery(vector<reader::Stop>& stops, vector<reader::Bus>& buses)
    {
        string str;
        size_t num;
        cin >> num;
        getline(cin, str);
        for (size_t i = 0; i < num; ++i)
        {
            getline(cin, str);
            if (str.find("Stop") == 0)
            {
                stops.push_back(ParserStringStop(str));
            }
            else if (str.find("Bus") == 0)
            {
                buses.push_back(ParserStringBus(str));
            }
        }
    }

    void ReadQueries(vector<reader::QueryBusStops>& queryes)
    {
        string str;
        size_t num2;
        cin >> num2;
        getline(cin, str);
        for (size_t i = 0; i < num2; ++i)
        {
            getline(cin, str);
            if (str.find("Bus") == 0)
            {
                queryes.push_back(ParseQueryBus(str));
            }
            else if (str.find("Stop") == 0)
            {
                queryes.push_back(ParseQueryBusStops(str));
            }
        }
    }

    information::Catalogue ReadData()
    {
        vector<reader::Stop> inPutStops;
        vector<reader::Bus> inPutBus;
        reader::ReadDataQuery(inPutStops, inPutBus);

        information::Catalogue catalogue;

        for (auto& elem : inPutStops)
        {
            catalogue.AddStops(elem._stop, elem._lat, elem._lng, elem._distanceToStops);
        }
        for (auto& elem : inPutBus)
        {
            catalogue.AddBus(elem._numberBus, elem._stopBus, elem._isCircleOrNot);
        }
        return catalogue;
    }
}