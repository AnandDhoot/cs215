#include <bits/stdc++.h>
#include <sstream>

using namespace std;

map < string , map < string,vector<double> > > know_base;
map < string , string > country_id_map;
map < string,vector<string> > keywords;

vector<string> tokenize(string input)
{
	vector<string> result;
	string str = input;

	int start = 0, end = 0;
	for(int i=0; i<str.length(); i++)
	{
		if(str[i] == '\t')
		{
			end = i;
			string wrd = str.substr(start, end - start);
			result.push_back(wrd);
			start = i+1;
		}
	}
	string wrd = str.substr(start, end - start);
	result.push_back(wrd);

	return result;
}
vector<string> tokenizeCountries(string input)
{
	vector<string> result;
	set<string> unique;
	string str = input;

	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	while(str.find(',') != string::npos)
	{
		size_t pos = str.find(',');
		unique.insert(str.substr(0,pos));
		str = str.substr(pos+1, str.length() - pos - 1);
	}
	unique.insert(str);

	for(set<string>::iterator it = unique.begin(); it != unique.end(); it++)
		result.push_back(*it);
	return result;
}
vector<double> tokenizeNumbers(string input)
{
	vector<double> result;
	set<double> unique;
	string str = input;

	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	while(str.find(',') != string::npos)
	{
		size_t pos = str.find(',');
		stringstream ss;
		ss << str.substr(0,pos);
		double val;
		ss >> val;
		unique.insert(val);

		str = str.substr(pos+1, str.length() - pos - 1);
	}
	stringstream ss1(str);
	double val1;
	ss1 >> val1;
	unique.insert(val1);

	for(set<double>::iterator it = unique.begin(); it != unique.end(); it++)
		result.push_back(*it);

	return result;
}
void findResults(string Sid, vector<string> countries, vector<double> numbers,vector<string> words)
{
	int c_score;
	bool do_keywords=true;
	for(int i=0;i<countries.size();i++)
	{
		for(int j=0;j<numbers.size();j++)
		{
			map< string,vector<double> > temp = know_base[country_id_map[countries[i]]];
			for(map< string,vector<double> >::iterator p= temp.begin(); p != temp.end(); p++)
			{
				double max_conf=0;
				int max_k;
				double max_x,max_x1,max_diff;
				for(int k=0; k<((*p).second.size()); k++)
				{
					double x1=numbers[j];
					double x=(*p).second[k];
					double diff;
					double xm=max(x,x1);
					if(xm==0)
						diff= abs((x1-x)/(xm + 1));
					else
						diff= abs((x1-x)/(xm));
					double conf = exp(-1*pow(diff*30,2)/2) * 100;
					if(conf>max_conf)
					{
						max_k=k;
						max_conf=conf;
						max_x=x;
						max_diff=diff;
						max_x1=x1;
					}
				}
				if(max_conf>=20)
				{
					int match=0;
					vector<string> temp1=keywords[(*p).first];
					for(int i1=0;i1<temp1.size();i1++)
					{
						if(words[1].find(temp1[i1]) != string::npos)
						{
							match++;
						}
					}

					double conf1=max_conf+(100-max_conf)/3;
					double conf2=conf1+(100-conf1)/3;

					if(match==0 && max_conf>=50)
						cout<<Sid<<", "<<countries[i]<<", "<<(*p).first<<", "<<max_conf<< "%,\n";
					else if(match==1 && conf1>=50)
						cout<<Sid<<", "<<countries[i]<<", "<<(*p).first<<", "<<conf1<< "%,\n";
					else if(match>1)
					{
						for(int i1=3;i1<=match;i1++)
							conf2=conf2+(100-conf2)/3;
						if(conf2>=50)
							cout<<Sid<<", "<<countries[i]<<", "<<(*p).first<<", "<<conf1<< "%,\n";
					}
				}
			}
		}
	}
}
int main()
{

	ifstream kb("knowledgebase/kb-facts-train_SI.tsv");
	if(!kb)
	{
		cout<<"File not found.\n";
		return 0;
	}
	string line;
	while(getline(kb,line))
	{
		stringstream  lineStream(line);
		string country_id;
		lineStream>>country_id;
		double info;
		lineStream>>info;
 		string relation;
		lineStream>>relation;
		know_base[country_id][relation].push_back(info);
	}
	kb.close();

	ifstream c_id_file("knowledgebase/countries_id_map.txt");
	
	if(!c_id_file)
	{
		cout<<"File not found.\n";
		return 0;
	}

	while(getline(c_id_file,line))
	{
		stringstream lineStream(line);
		string country_id;
		string country_name;
		lineStream>>country_id;
		lineStream>>country_name;
		country_id_map[country_name] = country_id;
	}

	c_id_file.close();

	
	ifstream kw("keywords.txt");
	while(getline(kw, line))
	{
		vector<string> words = tokenize(line);
		for(int i=1;i<words.size();i++)
			keywords[words[0]].push_back(words[i]);
		
	}
	kw.close();

	ifstream in;
	cout<<"SentenceID, Country, Relation, Confidence Score\n";
	in.open("sentences.tsv");
	if(!in)
	{
		cout<<"File not found\n";
		return 0;
	}
	while(getline(in, line))
	{
		vector<string> words = tokenize(line);
		vector<string> countries = tokenizeCountries(words[3]);
		vector<double> numbers = tokenizeNumbers(words[2]);
		string Sid=words[0];

		findResults(Sid,countries, numbers, words);


	}

	in.close();
	return 0;
}