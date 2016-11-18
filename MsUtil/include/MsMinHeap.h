#ifndef __MS_MIN_HEAP_H__
#define __MS_MIN_HEAP_H__


namespace matchvs
{
	template <typename T>
	class MsMinHeap
	{
	public:
		MsMinHeap(int cap_ = 100);
		~MsMinHeap();
	public:
		bool insert(T val);
		bool remove(T data);
		T getTop();
		bool createMinHeap(T a[], int length);

	private:
		int capacity_;
		int size_;
		T * heap_;
	private:
		void filterUp(int index);
		void filterDown(int begin, int end);
	};

	template <typename T>
	MsMinHeap<T>::MsMinHeap(int cap_ = 100)
		:capacity_(cap_), size_(0), heap_(0)
	{
		heap_ = new T[capacity_];
	};

	template<typename T>
	MsMinHeap<T>::~MsMinHeap()
	{
		delete[] heap_;
	};

	template <typename T>
	T MsMinHeap<T>::getTop()
	{
		if (size_ != 0)
			return heap_[0];
	};

	template <typename T>
	bool MsMinHeap<T>::insert(T val)
	{
		if (size_ == capacity_)
		{
			T * heap_new = new T[2*capacity_];

			for (int i = 0; i < capacity_; i++)
			{
				heap_new[i] = heap_[i];
			}

			delete[] heap_;
			heap_ = NULL;
			heap_ = heap_new;
			capacity_ *= 2;
		}

		heap_[size_] = val;
		filterUp(size_);
		size_++;
		return true;
	};

	template <typename T>
	void MsMinHeap<T>::filterUp(int index)
	{
		T value = heap_[index];

		while (index > 0)
		{
			int indexParent = (index - 1) / 2;
			if (value >= heap_[indexParent])
				break;
			else
			{
				heap_[index] = heap_[indexParent];
				index = indexParent;
			}
		}
		heap_[index] = value;
	};

	template<typename T>
	bool MsMinHeap<T>::createMinHeap(T a[], int length)
	{
		if (length > capacity_)
			return false;
		for (int i = 0; i < length; i++)
		{
			insert(a[i]);
		}
		return true;
	};

	template<typename T>
	bool MsMinHeap<T>::remove(T data)
	{
		if (size_ == 0) 
			return false;
		int index;
		for (index = 0; index < size_; index++)
		{
			if (heap_[index] == data)
				break;
		}
		if (index == size_)
			return false;

		heap_[index] = heap_[size_ - 1];

		size_ -= 1;
		filterDown(index, size_);

		return true;
	};

	template<typename T>
	void MsMinHeap<T>::filterDown(int current, int end)
	{

		int child = current * 2 + 1;

		T value = heap_[current];

		while (child <= end - 1 )
		{
			if (child < end && heap_[child] > heap_[child + 1])
				child++;
			if (value < heap_[child])
				break;
			else
			{
				heap_[current] = heap_[child];
				current = child;
				child = child * 2 + 1;
			}
		}
		heap_[current] = value;
	};
};
#endif
