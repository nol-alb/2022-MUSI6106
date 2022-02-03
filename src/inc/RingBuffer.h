#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>
#include <cstring>

/*! \brief implement a circular buffer of type T
*/
template <class T>
class CRingBuffer
{
public:
    explicit CRingBuffer(int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadIdx(0),
        m_iWriteIdx(0),
        m_buffptr(0)
    
    {
        assert(iBufferLengthInSamples > 0);
        m_buffptr = new T[m_iBuffLength];
        

        // allocate and init
    }

    virtual ~CRingBuffer()
    {
        // free memory
        delete [] m_buffptr;
        m_buffptr=0;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc (T tNewValue)
    {
        put(tNewValue);
        m_iWriteIdx = wraparound(++m_iWriteIdx);
        
        
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_buffptr[m_iWriteIdx]= tNewValue;
    }
    
    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc()
    {
        float new_value = m_buffptr[m_iReadIdx];
        m_iReadIdx = wraparound(++m_iReadIdx);
        
        return new_value;
    }

    /*! return the value at the current read index
    \return float the value from the read index
    */
    T get() const
    {
        return m_buffptr[m_iReadIdx];
    }
    
    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        m_iReadIdx=0;
        m_iWriteIdx=0;
        for(int i =0; i<=m_iBuffLength;i++)
        {
            m_buffptr[i]=0;
        }
        
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWriteIdx;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        m_iWriteIdx=iNewWriteIdx;
        if (m_iWriteIdx<0 || m_iWriteIdx>=m_iBuffLength){
            std::cout<<"Check Write Index, cannot be negative or over the buffer length"<<std::endl;
        }
        else{
        
        }
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadIdx;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx(int iNewReadIdx)
    {
        m_iReadIdx=iNewReadIdx;
        if (m_iReadIdx<0 || m_iReadIdx>=m_iBuffLength){
            std::cout<<"Check the read Index, no negative values or over the buffer length"<<std::endl;
        }
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        // (12-8+15)
        return (m_iWriteIdx-m_iReadIdx + m_iBuffLength)%m_iBuffLength ;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength() const
    {
        return -1;
    }
private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    int m_iReadIdx;
    int m_iWriteIdx;
    
    T *m_buffptr;
    int wraparound(int Idx) {
        int Idx_new = Idx%m_iBuffLength;
        return Idx_new;
        }
    
};
#endif // __RingBuffer_hdr__


