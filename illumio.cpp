#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;
// Tree node each contains port range and valid IP range
class PortNode{
private:
	int port_upper_;
	int port_lower_;
	PortNode* left_, *right_;
	// set<pair<unsigned int,unsigned int>> IP_range_;
public:
	set<pair<unsigned int,unsigned int>> IP_range_;
	PortNode(int port_upper, int port_lower): port_upper_(port_upper), port_lower_(port_lower), left_(NULL), right_(NULL){}
	void merge_IP_range_(unsigned int ip_upper, unsigned int ip_lower){
		vector<pair<int,int>> garbage_v;
		for(auto& item : IP_range_){
			if((ip_lower <= (item.second + 1)) &&  ((ip_upper + 1) >= item.first)) {
				garbage_v.push_back(item);
				ip_lower = min(ip_lower , item.first);
				ip_upper = max(ip_upper, item.second);
			}
		}
		for(auto& item : garbage_v) {
			IP_range_.erase(item);
		}
		IP_range_.insert(make_pair(ip_lower, ip_upper));
	}

	void add_IP_range_(int port_upper, int port_lower, unsigned int ip_upper, unsigned int ip_lower){
		// base case
		if((port_upper == port_upper_) && (port_lower == port_lower_)){
			merge_IP_range_(ip_upper, ip_lower);
			return;
		}
		int left_upper = (port_upper_-port_lower_) / 2 + port_lower_;
		int right_lower = left_upper + 1;
		// check left
		if(port_lower <= left_upper){
			int next_port_upper = port_upper < left_upper ? port_upper : left_upper;
			left_->add_IP_range_(next_port_upper, port_lower, ip_upper, ip_lower);
		}
		// check right
		if(port_upper >= right_lower){
			int next_port_lower = port_lower < right_lower ? right_lower : port_lower;
			right_->add_IP_range_(port_upper, next_port_lower, ip_upper, ip_lower);
		}

	}

	void set_left_right(PortNode *left = NULL, PortNode *right = NULL){
		left_ = left;
		right_ = right;
	}

	bool have_match(int port, unsigned int ip) {
		for(auto& it : IP_range_){
			if((ip >= it.first) && (ip <= it.second)){
				return true;
			}
		}
		return false; // to do to compare ip
	}

	bool find_match(int port, unsigned int ip, PortNode * root) {
		// base case
		if(root == NULL) return false;
		
		if(! root->have_match(port, ip)) {
			int left_max = (root->get_port_upper() - root->get_port_lower()) / 2 + root->get_port_lower();
			if(port <= left_max) {
				return find_match(port, ip, root->get_left_());
			} else {
				return find_match(port, ip, root->get_right_());
			}
		}
		return true;

	}

	PortNode * get_left_() { return left_;}
	PortNode * get_right_() { return right_;}
	int get_port_upper() {return port_upper_;}
	int get_port_lower() {return port_lower_;}
};


// This function create a port tree with each node contains a port range and valid IP address
PortNode* constructSearchTree(int min_port = 1, int max_port = 65535){
	// base case
	if(min_port == max_port){
		return new PortNode(max_port, min_port);
	}
	PortNode* cur = new PortNode(max_port, min_port);
	cur->set_left_right(constructSearchTree(min_port, (max_port-min_port) / 2 + min_port), 
						constructSearchTree((max_port-min_port) / 2 + min_port + 1, max_port));
	return cur;
}

class Firewall{
private:
	PortNode* InboundTCP;
	PortNode* InboundUDP;
	PortNode* OutboundTCP;
	PortNode* OutboundUDP;
public:
	Firewall(string path){
		// construct four trees representing Inbound/tcp, Inbound/udp, Outbound/tcp, outbound/udp
		this->InboundTCP = constructSearchTree(1, 65535);
		this->InboundUDP = constructSearchTree(1, 65535);
		this->OutboundTCP = constructSearchTree(1, 65535);
		this->OutboundUDP = constructSearchTree(1, 65535);
		ifstream rules(path);
		string line;
		while(getline(rules, line)){
			string delimeter = ",";
			// four_columns represents direction, protocol, port, ip
			vector<string> four_columns;
			size_t pos = 0;
			string token;
			while((pos = line.find(delimeter)) != string::npos){
				token = line.substr(0, pos);
				four_columns.push_back(token);
				line.erase(0, pos + 1);
			}
			four_columns.push_back(line);
			// convert port number or port range to integer
			delimeter = "-";
			int port_lower, port_upper;
			if((pos = four_columns[2].find(delimeter)) != string::npos){
				port_lower = stoi(four_columns[2].substr(0, pos));
				port_upper = stoi(four_columns[2].substr(pos+1, string::npos));
			}
			else{
				port_lower = port_upper = stoi(four_columns[2]);
			}
			// convert ip number or range to integer
			//(first << 24) | (second << 16) | (third << 8) | (fourth)
			unsigned int ip_lower, ip_upper;
			if((pos = four_columns[3].find(delimeter)) != string::npos){
				string first_ip = four_columns[3].substr(0, pos);
				string second_ip = four_columns[3].substr(pos+1, string::npos);
				vector<int> first_ip_four_part;
				vector<int> second_ip_four_part;
				while((pos = first_ip.find(".")) != string::npos){
					first_ip_four_part.push_back(stoi(first_ip.substr(0, pos)));
					first_ip.erase(0, pos + 1);
				}
				first_ip_four_part.push_back(stoi(first_ip));
				while((pos = second_ip.find(".")) != string::npos){
					second_ip_four_part.push_back(stoi(second_ip.substr(0, pos)));
					second_ip.erase(0, pos + 1);
				}
				second_ip_four_part.push_back(stoi(second_ip));
				ip_lower = (first_ip_four_part[0] << 24) | (first_ip_four_part[1] << 16) | (first_ip_four_part[2] << 8) | (first_ip_four_part[3]);
				ip_upper = (second_ip_four_part[0] << 24) | (second_ip_four_part[1] << 16) | (second_ip_four_part[2] << 8) | (second_ip_four_part[3]);
			}
			else{
				vector<int> ip_four_part;
				string ip = four_columns[3];
				while((pos = ip.find(".")) != string::npos){
					ip_four_part.push_back(stoi(ip.substr(0, pos)));
					ip.erase(0, pos + 1);
				}
				ip_four_part.push_back(stoi(ip));
				ip_lower = ip_upper = (ip_four_part[0] << 24) | (ip_four_part[1] << 16) | (ip_four_part[2] << 8) | (ip_four_part[3]);
			}		
			// Add IP range depending on direction and protocol
			if(four_columns[0] == "inbound" && four_columns[1] == "tcp"){
				this->InboundTCP->add_IP_range_(port_upper, port_lower, ip_upper, ip_lower);
			}
			else if(four_columns[0] == "inbound" && four_columns[1] == "udp"){
				this->InboundUDP->add_IP_range_(port_upper, port_lower, ip_upper, ip_lower);
			}
			else if(four_columns[0] == "outbound" && four_columns[1] == "tcp"){
				this->OutboundTCP->add_IP_range_(port_upper, port_lower, ip_upper, ip_lower);
			}
			else if(four_columns[0] == "outbound" && four_columns[1] == "udp"){
				this->OutboundUDP->add_IP_range_(port_upper, port_lower, ip_upper, ip_lower);
			}
		}
	}

	bool accept_packet(string direction, string protocol, int port, string ip_address){
		vector<int> ip_four_part;
		size_t pos;
		while((pos = ip_address.find(".")) != string::npos){
			ip_four_part.push_back(stoi(ip_address.substr(0, pos)));
			ip_address.erase(0, pos + 1);
		}
		ip_four_part.push_back(stoi(ip_address));
		unsigned int ip = (ip_four_part[0] << 24) | (ip_four_part[1] << 16) | (ip_four_part[2] << 8) | (ip_four_part[3]);
		if(direction == "inbound" && protocol == "tcp"){
			return this->InboundTCP->find_match(port, ip, this->InboundTCP);
		}
		else if(direction == "inbound" && protocol == "udp"){
			return this->InboundUDP->find_match(port, ip, this->InboundUDP);
		}
		else if(direction == "outbound" && protocol == "tcp"){
			return this->OutboundTCP->find_match(port, ip, this->OutboundTCP);
		}
		else{
			return this->OutboundUDP->find_match(port, ip, this->OutboundUDP);
		}
	}
};

int main(){
	string path = "/Users/ningkaiwu/Desktop/leetcode/illumio/test3.csv";
	Firewall* fw = new Firewall(path);
	cout<<fw->accept_packet("inbound", "tcp", 80, "192.168.1.2") <<endl;
	cout<<fw->accept_packet("inbound", "udp", 53, "192.168.2.1") <<endl;
	cout<<fw->accept_packet("outbound", "tcp", 10234, "192.168.10.11") <<endl;
	cout<<fw->accept_packet("inbound", "tcp", 81, "192.168.1.2") <<endl;
	cout<<fw->accept_packet("inbound", "udp", 24, "52.12.48.92") <<endl;
	cout<<fw->accept_packet("inbound", "tcp", 65, "192.168.1.12") <<endl;
	cout<<fw->accept_packet("inbound", "tcp", 65, "192.168.1.254") <<endl;
	cout<<fw->accept_packet("inbound", "tcp", 65, "192.168.1.255") <<endl;
	return 0;
}