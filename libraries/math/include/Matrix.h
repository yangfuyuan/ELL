////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"

namespace math
{
    /// <summary> Enum of possible matrix layouts. </summary>
    enum class MatrixLayout { columnMajor,  rowMajor };

    /// <summary> Forward declaration of a base class for matrices, for subsequent specialization according to layout. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Type of the layout. </typeparam>
    template<typename ElementType, MatrixLayout Layout>
    class MatrixBase;

    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase
    {
    public:

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _numRows; }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _numColumns; }

    protected:
        // allow operations defined in the Operations struct to access raw data vector
        RectangularMatrixBase(ElementType* pData, size_t numRows, size_t numColumns) : _pData(pData), _numRows(numRows), _numColumns(numColumns)
        {}

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
    };

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns), _columnIncrement(numRows)
        {}

        size_t GetIndex(size_t rowIndex, size_t columnIndex) const
        {
            // TODO check indices
            return rowIndex + columnIndex * _columnIncrement;
        }

        MatrixLayout GetLayout() const
        {
            return MatrixLayout::columnMajor;
        }

        size_t GetRowIncrement()
        {
            return 1;
        }

        size_t GetColumnIncrement()
        {
            return _columnIncrement;
        }

    private:
        size_t _columnIncrement;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns), _rowIncrement(numColumns)
        {}

        size_t GetIndex(size_t rowIndex, size_t columnIndex) const
        {
            // TODO check indices
            return rowIndex * _rowIncrement + columnIndex;
        }

        MatrixLayout GetLayout() const
        {
            return MatrixLayout::rowMajor;
        }

        size_t GetRowIncrement() const
        {
            return _rowIncrement;
        }

        size_t GetColumnIncrement() const
        {
            return 1;
        }

    private:
        size_t _rowIncrement;
    };

    /// <summary> Const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class ConstMatrixReference : public MatrixBase<ElementType, Layout>
    {
    public:

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        ElementType operator() (size_t rowIndex, size_t columnIndex)  const
        { 
            return _pData[GetIndex(rowIndex, columnIndex)];
        }

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        ConstMatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
        {
            // TODO check inputs
            return ConstMatrixReference<ElementType, Layout>(_pData + GetIndex(firstRow, firstColumn), numRows, numColumns, _increment);
        }

        ConstVectorReference<ElementType, VectorOrientation::column> GetColumn(size_t index) const
        {
            return ConstVectorReference<ElementType, VectorOrientation::column>();
        }



        bool operator==(const ConstMatrixReference<ElementType, Layout>& other) const // TODO, compare matrices with different layout
        {
            if(NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
            {
                return false;
            }


            return true;
        }

    protected:
        using MatrixBase<ElementType, Layout>::MatrixBase;

    };

    /// <summary> Non-const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class MatrixReference : public ConstMatrixReference<ElementType, Layout>
    {
    public:
        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        ElementType& operator() (size_t rowIndex, size_t columnIndex)  
        { 
            return _pData[GetIndex(rowIndex, columnIndex)];
        }

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        MatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
        {
            // TODO check inputs
            return MatrixReference<ElementType, Layout>(_pData + GetIndex(firstRow, firstColumn), numRows, numColumns, _increment);
        }

    protected:
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;
    };


    /// <summary> A matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class Matrix : public MatrixReference<ElementType, Layout>
    {
    public:

        /// <summary> Constructs an all-zeros matrix of a given size. </summary>
        ///
        /// <param name="numRows"> Number of rows in the matrix. </param>
        /// <param name="numColumns"> Number of columns in the matrix. </param>
        Matrix(size_t numRows, size_t numColumns) : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(numRows*numColumns) 
        {
            _pData = _data.data();
        }

        Matrix(std::initializer_list<std::initializer_list<ElementType>> list) : MatrixReference<ElementType, Layout>(list.size(), list.begin()->size()), _data(list.size() * list.begin()->size())
        {
            _pData = _data.data();
           
            size_t i = 0;
            for(auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
            {
                // check that the length of the row is the same as NumColumns();

                size_t j = 0;
                for(auto elementIter = rowIter->begin(); elementIter < rowIter->end(); ++elementIter)
                {
                    (*this)(i, j) = *elementIter;
                    ++j;
                }
                ++i;
            }
        }

    private:
        std::vector<ElementType> _data;
    };

    //
    // friendly names
    // 
    typedef Matrix<double, MatrixLayout::columnMajor> DoubleColumnMatrix;
    typedef Matrix<double, MatrixLayout::rowMajor> DoubleRowMatrix;
}

#include "../tcc/Matrix.tcc"
