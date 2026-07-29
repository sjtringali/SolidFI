// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Deferred } from './Deferred';
import { Converter } from './Converter';
import { Chain } from './Chain';

class UpperCase implements Converter<string, string> {
    resolve(value: string): string {
        return value.toUpperCase();
    }
}

describe('Deferred', () => {
    it('delegates resolve() to the constructed converter', () => {
        const d = new Deferred(UpperCase);
        assert.equal(d.resolve('hello', {}), 'HELLO');
        assert.equal(d.resolve('world', {}), 'WORLD');
    });

    it('acquire() resolves via the constructed converter', () => {
        const d = new Deferred(UpperCase);
        assert.equal(d.acquire('hello', {}), 'HELLO');
    });

    it('is assignable to Converter<T, U>', () => {
        const d: Converter<string, string> = new Deferred(UpperCase);
        assert.equal(d.resolve('hi', {}), 'HI');
    });

    it('can be installed in a Chain instead of a direct instance', () => {
        const chain = new Chain<string, string | null>(null);
        chain.install(1, 'upper', new Deferred(UpperCase));
        assert.equal(chain.resolve('hello', {}), 'HELLO');
        assert.equal(chain.resolve('world', {}), 'WORLD');
    });
});
