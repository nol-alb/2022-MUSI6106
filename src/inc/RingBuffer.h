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
    \param fOffset: read at offset from read index
    \return float the value from the read index
     //const means the input parameters to the method will not be changed
    */
    T get(float fOffset = 0) const
    {
        //If offset is zero it is a normal get
        if (fOffset==0)
        {
            return m_buffptr[m_iReadIdx]
        }
        else
        {
            //interp = (1-frac)*m_ptBuff[i(low)] + frac*m_ptBuff[i(high)];
            // seperate the int and fractional part
            // Use the int part to move ahead in the m_iReadIdx, handle negative values
                //Check for Wraparound in this stage
            //Then increment a new iReadIdx which is incremented
            //Use interpolation formula using the fractional part

       if (fOffset)
           return m_ptBuff[m_iReadIdx];
       else
       {
           // Get Fractional Part
           int IntOffset = static_cast<int>(floor(fOffset));
           float fracOffset = fOffset-IntOffset;

           int new_Read = incIdx(m_iReadIdx+IntOffset,0);
           // Check Negative Values and return
           new_Read = AllPos(new_Read);
           float cur_BuffVal = m_ptBuff[new_Read];
           float nex_BuffVal = m_ptBuff[incIdx(new_Read+1,0)];
           //Weighted sum
           return (1-fracOffset)*cur_BuffVal + fracOffset*(nex_BuffVal);
       }
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
    int AllPos(int ReadIdx)
    {
        if(ReadIdx<0) {
            return AllPos(ReadIdx + m_iBuffLength);
        }
        else if(ReadIdx > m_iBuffLength-1) {
            return AllPos(ReadIdx - m_iBuffLength);
        }
        else {
            return incIdx(ReadIdx, 0);
        }
    }
    
};
#endif // __RingBuffer_hdr__


