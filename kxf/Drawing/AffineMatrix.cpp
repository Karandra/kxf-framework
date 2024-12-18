#include "kxf-pch.h"
#include "AffineMatrix.h"
#include <wx/affinematrix2d.h>

namespace kxf::Private
{
	AffineMatrixD FromWxAffineMatrix(const wxAffineMatrix2D& matrix) noexcept
	{
		wxMatrix2D matrix2D;
		wxPoint2DDouble txy;
		matrix.Get(&matrix2D, &txy);

		return
		{
			static_cast<double>(matrix2D.m_11),
			static_cast<double>(matrix2D.m_12),
			static_cast<double>(matrix2D.m_21),
			static_cast<double>(matrix2D.m_22),
			static_cast<double>(txy.m_x),
			static_cast<double>(txy.m_y)
		};
	}
	wxAffineMatrix2D ToWxAffineMatrix(const AffineMatrixD& matrix) noexcept
	{
		double m11 = 0;
		double m12 = 0;
		double m21 = 0;
		double m22 = 0;
		double tx = 0;
		double ty = 0;
		matrix.GetElements(m11, m12, m21, m22, tx, ty);

		wxMatrix2D matrix2D(static_cast<wxDouble>(m11), static_cast<wxDouble>(m12), static_cast<wxDouble>(m21), static_cast<wxDouble>(m22));
		wxPoint2DDouble txy(static_cast<wxDouble>(tx), static_cast<wxDouble>(ty));

		wxAffineMatrix2D affineMatrix;
		affineMatrix.Set(matrix2D, txy);
		return affineMatrix;
	}
}
