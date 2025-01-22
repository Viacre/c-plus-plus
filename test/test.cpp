#include "cxx/vector.hpp"
#include "cxx/array.hpp"
#include <iostream>
#include <string>
#include <span>
#include <deque>
#include <array>

namespace
{

using std::cout;
using std::endl;
using std::cerr;
using std::exception;

namespace vector_test
{

using cxx::vector;
using std::string;

template<typename _tVector>
void
println(const _tVector& v)
{
	cout << "result: { ";
	for(const auto& i: v)
		cout << i << ' ';
	cout << "}\nsize: " << v.size() << " capacity: " << v.capacity() << '\n'
		 << endl;
}

void
test()
{
	cout << "vector test:\n";
	vector<int> v;
	v.assign(16, 1);
	println(v);
	v.assign({1, 2, 3, 4, 5, 6});
	println(v);
	int arr[]{10, 2, 3};
	v.assign(arr, arr + 3);
	println(v);

	vector<int> v2(16);
	v2.assign(16, 6);
	v2[15] = 100;
	cout << *(v2.cend() - 1) << endl;

	const vector<int> v3(16);
	try
	{
		cout << v3.at(16);
	}
	catch(const exception& e)
	{
		cerr << e.what() << endl;
	}
	cout << v2.max_size() << endl;

	cout << *v2.data() << endl;

	cout << v2.front() << ' ' << v2.back() << ' ' << v3.front() << ' '
		 << v3.back() << '\n';

	vector<int> v4(v);
	println(v4);

	vector<int> v5(8, 66);
	println(v5);

	v4 = v5;
	println(v4);

	v4 = std::move(v5);
	println(v4);

	v4 = {1, 11, 111, 111, 2, 22, 222};
	println(v4);

	vector<int> v6;
	v6.push_back(10);
	println(v6);
	v6.emplace_back(6);
	println(v6);

	vector<string> v7;
	string str{"66"};

	v7.push_back("77");
	v7.push_back("第一");
	v7.push_back(str);
	v7.push_back(string("正面"));
	println(v7);
	v7.pop_back();
	v7.push_back("反面");
	println(v7);

	v7.resize(10, "*");
	println(v7);

	cout << std::distance(v7.cbegin(), v7.cend()) << endl;

	v7.emplace(v7.cend(), string("78"));
	v7.emplace(v7.begin(), string("*****"));
	v7.insert(v7.begin(), "*66*");

	println(v7);
	v7.insert(v7.end(), 10, "哈");
	println(v7);

	int array2[]{1, 2, 3, 4, 5};
	v6.insert(v6.begin() + 2, std::begin(array2), std::end(array2));
	println(v6);

	v6.insert(v6.end(), {9, 9, 9, 9, 9, 9, 9});
	println(v6);
	v6.erase(v6.begin());
	println(v6);
	v6.erase(v6.begin(), v6.begin() + 6);
	println(v6);

	// vector<int> v8;
	// v8.swap(v6);

	// cout << v8.size() << ' ' << v8.capacity() << '\n';
	// cout << v6.size() << ' ' << v6.capacity() << '\n';
	// println(v8);

	vector<int> v9{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	println(v9);

	vector<int> v10(v9.begin(), v9.begin() + 5);
	println(v10);

	// cout << (v10 == v9) << ' ' << (v10 != v9) << '\n';
	// cout << (v10 < v9) << '\n';
	// cout << (v10 >= v9) << ' ' << (v10 <= v9) << '\n';
	// cout << int((v10 <=> v9)._Value) << endl;

	// swap(v10, v9);
	// cout << int((v10 <=> v9)._Value) << endl;

	// println(v10);
}


} // namespace vector_test

namespace array_test
{

using cxx::array;

template<typename _tArray>
void
println(const _tArray& arr)
{
	cout << "result: { ";
	for(const auto& i: arr)
		cout << i << ' ';
	cout << "}" << endl;
}

void
test()
{
	cout << "Array Test\n";
	array<int, 10> arr{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	try
	{
		cout << arr.at(10) << endl;
	}
	catch(const exception& e)
	{
		cerr << e.what() << endl;
	}
	cout << arr.at(9) << endl;
	cout << arr[9] << endl;
	arr[9] = 66;

	const array<int, 10> arr1{1, 2};
	cout << arr1[1] << ' ' << arr1.at(0) << endl;

	arr1[0];

	cout << (*arr.begin() == *arr.rend()) << endl; //0
	cout << *arr.cbegin() << ' ' << *(arr.crend() - 1) << endl; //1 1
	cout << *arr.rbegin() << ' ' << *arr.crbegin() << ' ' << *(arr.end() - 1)
		 << endl; //66 66 66

	arr.fill(1);
	println(arr);

	array<int, 10> arr2;
	arr2.fill(2);
	arr.swap(arr2);

	println(arr2);

	// auto& ret = get<1>(arr);
	// cout << ret << endl;

	int arr3[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto arr4 = cxx::to_array<int>(arr3);
	println(arr4);

	auto arr5 = cxx::to_array<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
	println(arr5);

	array<int, 6> arr6{1, 2, 3, 4, 5, 6};
	println(arr6);
}

} // namespace array_test

} // unnamed namespace

int
main()
{
	using std::array;
	using std::deque;

	vector_test::test();
	array_test::test();
}
