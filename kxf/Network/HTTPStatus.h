#pragma once
#include "Common.h"
#include "kxf/Core/IErrorCode.h"
#include "kxf/Core/IndexedEnum.h"

namespace kxf
{
	enum class HTTPStatusCode: uint32_t
	{
		Unknown = 0,

		Continue = 100,
		SwitchingProtocols = 101,
		Processing = 102,
		EarlyHints = 103,

		Success = 200,
		Created = 201,
		Accepted = 202,
		NonAuthoritativeInformation = 203,
		NoContent = 204,
		ResetContent = 205,
		PartialContent = 206,
		MultiStatus = 207,
		AlreadyReported = 208,
		IMUsed = 226,

		MultipleChoices = 300,
		MovedPermanently = 301,
		Found = 302,
		SeeOther = 303,
		NotModified = 304,
		UseProxy = 305,
		SwitchProxy = 306,
		TemporaryRedirect = 307,
		PermanentRedirect = 308,

		BadRequest = 400,
		Unauthorized = 401,
		PaymentRequired = 402,
		Forbidden = 403,
		NotFound = 404,
		MethodNotAllowed = 405,
		NotAcceptable = 406,
		ProxyAuthenticationRequired = 407,
		RequestTimeout = 408,
		Conflict = 409,
		Gone = 410,
		LengthRequired = 411,
		PreconditionFailed = 412,
		PayloadTooLarge = 413,
		URITooLong = 414,
		UnsupportedMediaType = 415,
		RangeNotSatisfiable = 416,
		ExpectationFailed = 417,
		ImATeapot = 418,
		UnprocessableEntity = 422,
		Locked = 423,
		FailedDependency = 424,
		UpgradeRequired = 426,
		PreconditionRequired = 428,
		TooManyRequests = 429,
		RequestHeaderFieldsTooLarge = 431,
		UnavailableForLegalReasons = 451,

		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503,
		GatewayTimeout = 504,
		HTTPVersionNotSupported = 505,
		VariantAlsoNegotiates = 506,
		InsufficientStorage = 507,
		LoopDetected = 508,
		NotExtended = 510,
		NetworkAuthenticationRequired = 511
	};
}

namespace kxf
{
	struct HTTPStatusCodeDef final: public IndexedEnumDefinition<HTTPStatusCodeDef, HTTPStatusCode, std::string_view>
	{
		inline static constexpr TItem Items[] =
		{
			{HTTPStatusCode::Continue, "Continue"},
			{HTTPStatusCode::SwitchingProtocols, "SwitchingProtocols"},
			{HTTPStatusCode::Processing, "Processing"},
			{HTTPStatusCode::EarlyHints, "EarlyHints"},

			{HTTPStatusCode::Success, "Success"},
			{HTTPStatusCode::Created, "Created"},
			{HTTPStatusCode::Accepted, "Accepted"},
			{HTTPStatusCode::NonAuthoritativeInformation, "NonAuthoritativeInformation"},
			{HTTPStatusCode::NoContent, "NoContent"},
			{HTTPStatusCode::ResetContent, "ResetContent"},
			{HTTPStatusCode::PartialContent, "PartialContent"},
			{HTTPStatusCode::MultiStatus, "MultiStatus"},
			{HTTPStatusCode::AlreadyReported, "AlreadyReported"},
			{HTTPStatusCode::IMUsed, "IMUsed"},

			{HTTPStatusCode::MultipleChoices, "MultipleChoices"},
			{HTTPStatusCode::MovedPermanently, "MovedPermanently"},
			{HTTPStatusCode::Found, "Found"},
			{HTTPStatusCode::SeeOther, "SeeOther"},
			{HTTPStatusCode::NotModified, "NotModified"},
			{HTTPStatusCode::UseProxy, "UseProxy"},
			{HTTPStatusCode::SwitchProxy, "SwitchProxy"},
			{HTTPStatusCode::TemporaryRedirect, "TemporaryRedirect"},
			{HTTPStatusCode::PermanentRedirect, "PermanentRedirect"},

			{HTTPStatusCode::BadRequest, "BadRequest"},
			{HTTPStatusCode::Unauthorized, "Unauthorized"},
			{HTTPStatusCode::PaymentRequired, "PaymentRequired"},
			{HTTPStatusCode::Forbidden, "Forbidden"},
			{HTTPStatusCode::NotFound, "NotFound"},
			{HTTPStatusCode::MethodNotAllowed, "MethodNotAllowed"},
			{HTTPStatusCode::NotAcceptable, "NotAcceptable"},
			{HTTPStatusCode::ProxyAuthenticationRequired, "ProxyAuthenticationRequired"},
			{HTTPStatusCode::RequestTimeout, "RequestTimeout"},
			{HTTPStatusCode::Conflict, "Conflict"},
			{HTTPStatusCode::Gone, "Gone"},
			{HTTPStatusCode::LengthRequired, "LengthRequired"},
			{HTTPStatusCode::PreconditionFailed, "PreconditionFailed"},
			{HTTPStatusCode::PayloadTooLarge, "PayloadTooLarge"},
			{HTTPStatusCode::URITooLong, "URITooLong"},
			{HTTPStatusCode::UnsupportedMediaType, "UnsupportedMediaType"},
			{HTTPStatusCode::RangeNotSatisfiable, "RangeNotSatisfiable"},
			{HTTPStatusCode::ExpectationFailed, "ExpectationFailed"},
			{HTTPStatusCode::ImATeapot, "ImATeapot"},
			{HTTPStatusCode::UnprocessableEntity, "UnprocessableEntity"},
			{HTTPStatusCode::Locked, "Locked"},
			{HTTPStatusCode::FailedDependency, "FailedDependency"},
			{HTTPStatusCode::UpgradeRequired, "UpgradeRequired"},
			{HTTPStatusCode::PreconditionRequired, "PreconditionRequired"},
			{HTTPStatusCode::TooManyRequests, "TooManyRequests"},
			{HTTPStatusCode::RequestHeaderFieldsTooLarge, "RequestHeaderFieldsTooLarge"},
			{HTTPStatusCode::UnavailableForLegalReasons, "UnavailableForLegalReasons"},

			{HTTPStatusCode::InternalServerError, "InternalServerError"},
			{HTTPStatusCode::NotImplemented, "NotImplemented"},
			{HTTPStatusCode::BadGateway, "BadGateway"},
			{HTTPStatusCode::ServiceUnavailable, "ServiceUnavailable"},
			{HTTPStatusCode::GatewayTimeout, "GatewayTimeout"},
			{HTTPStatusCode::HTTPVersionNotSupported, "HTTPVersionNotSupported"},
			{HTTPStatusCode::VariantAlsoNegotiates, "VariantAlsoNegotiates"},
			{HTTPStatusCode::InsufficientStorage, "InsufficientStorage"},
			{HTTPStatusCode::LoopDetected, "LoopDetected"},
			{HTTPStatusCode::NotExtended, "NotExtended"},
			{HTTPStatusCode::NetworkAuthenticationRequired, "NetworkAuthenticationRequired"},
		};
	};
}

namespace kxf
{
	class HTTPStatus final: public RTTI::DynamicImplementation<HTTPStatus, IErrorCode>, public IndexedEnumValue<HTTPStatusCodeDef, HTTPStatusCode::Unknown>
	{
		kxf_RTTI_DeclareIID(HTTPStatus, {0xc9e3d90, 0xaf81, 0x48d8, {0x90, 0x25, 0xca, 0x15, 0x71, 0xec, 0x3d, 0x59}});

		public:
			HTTPStatus() noexcept = default;
			HTTPStatus(HTTPStatusCode value) noexcept
				:IndexedEnumValue(value)
			{
			}
			HTTPStatus(TInt value) noexcept
				:HTTPStatus(static_cast<HTTPStatusCode>(value))
			{
			}

			template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
			HTTPStatus(std::optional<T> value) noexcept
				:HTTPStatus(value ? static_cast<HTTPStatusCode>(*value) : HTTPStatusCode::Unknown)
			{
			}

		public:
			// IErrorCode
			bool IsSuccess() const noexcept override
			{
				// [200, 300)
				return IsInRange(200, 299);
			}
			bool IsFail() const noexcept override
			{
				// [400, +]
				return ToInt() >= 400;
			}

			uint32_t GetValue() const noexcept override
			{
				return IndexedEnumValue::ToInt();
			}
			void SetValue(uint32_t value) noexcept override
			{
				IndexedEnumValue::SetValue(static_cast<HTTPStatusCode>(value));
			}

			String ToString() const override
			{
				return IndexedEnumValue::ToString();
			}
			String GetMessage(const Locale& locale = {}) const override
			{
				// TODO: Add (localized if possible) error messages.
				return {};
			}

			// HTTPStatus
			HTTPStatusCode GetCode() const noexcept
			{
				return IndexedEnumValue::GetValue();
			}
			void SetCode(HTTPStatusCode value) noexcept
			{
				IndexedEnumValue::SetValue(value);
			}

			bool IsInformation() const noexcept
			{
				// [100, 200)
				return IsInRange(100, 199);
			}
			bool IsRedirection() const noexcept
			{
				// [300, 400)
				return IsInRange(300, 399);
			}
			bool IsClientError() const noexcept
			{
				// [400, 500)
				return IsInRange(400, 499);
			}
			bool IsServerError() const noexcept
			{
				// [500, 600)
				return IsInRange(500, 599);
			}

		public:
			HTTPStatus& AddFlag(TEnum value) noexcept = delete;
			HTTPStatus& RemoveFlag(TEnum value) noexcept = delete;
			HTTPStatus& ModFlag(TEnum value, bool condition) noexcept = delete;
			bool HasFlag(TEnum value) const noexcept = delete;
			bool HasSpecifiedFlagOnly(TEnum value) const noexcept = delete;

			explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			bool operator!() const noexcept
			{
				return IsFail();
			}
	};
}
